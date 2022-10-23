
#include "usart.h"
#include "tim.h"
#include "main.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "bootloader.h"
#include "variable.h"

enum{
    LSB_FIRST,
    MSB_FIRST
};
uint32_t SerialDataSort(uint8_t len, uint8_t* data, uint32_t endianness)
{
    int i;
    uint32_t ret_val = 0;
    if(endianness == MSB_FIRST)
    {
        for(i=0; i<len; i++)
            ret_val  |= (uint32_t)data[i] << (8*(len-i-1));
    }
    else
    {
        for(i=0; i<len; i++)
            ret_val  |= (uint32_t)data[i] << (8*i);
    }
    return ret_val;
}

uint8_t checksum(uint16_t len, uint8_t* data) 
{
	uint8_t cs = 0;
	for(int i=0; i<len; i++)
	{
		cs += data[i];
	}
	cs = ~cs + 1; //2's complment
	return cs;
}


uint32_t bootloader_state = 0;
uint32_t bootloader_program_state = 0;

typedef struct firmware_update{
    uint32_t address;
    uint32_t size;
    uint8_t version[3];
    uint32_t checksum;
}firmware_update_t;

enum{
    IDLE_STATE,
    FLASH_PROGRAM_STATE,
    JUMP_STATE
};
enum{
    CLEAR_VAR,
    FRAME_ID_CHECK,
    HEADER_GET,
    TRANSMIT,
    UPDATE_DONE
};
uint8_t transmit_buf[256] = {0};
void negative_response(void)
{
    transmit_buf[0] = BL_START_OF_FRAME;
    transmit_buf[1] = BL_NACK_FRAME;
    transmit_buf[2] = 0;
    transmit_buf[3] = 0; //length 0
    transmit_buf[4] = 0; //checksum
    transmit_buf[5] = BL_END_OF_FRAME; 
    HAL_UART_Transmit(&huart1,&transmit_buf[0],6,1000);
}
void positive_response(void)
{
    transmit_buf[0] = BL_START_OF_FRAME;
    transmit_buf[1] = BL_ACK_FRAME;
    transmit_buf[2] = 0;
    transmit_buf[3] = 0; //length 0
    transmit_buf[4] = 0; //checksum
    transmit_buf[5] = BL_END_OF_FRAME; 
    HAL_UART_Transmit(&huart1,&transmit_buf[0],6,1000);
}

#define DATA_LEN   2
#define DATA_BEGIN 4
uint16_t data_len = 0;
uint8_t frame_checksum = 0;
int i = 0;
firmware_update_t update_info = {0};
void flash_program(void)
{
    bootloader_program_state = FRAME_ID_CHECK;
    
    while(!(bootloader_flag&0x2))
    {
        switch (bootloader_program_state)
        {
        case CLEAR_VAR:
            bootloader_flag &= ~1; //clear bit 0
            recv_counter = 0;
            data_len = 0;
            memset((void *)recv_buf, 0x0, sizeof(frame_format_t));
            bootloader_program_state = FRAME_ID_CHECK;
            break;
        case FRAME_ID_CHECK:
            if(recv_counter >= 4)
            {
                if(recv_buf[1] == BL_HEADER)
                {
                    bootloader_program_state = HEADER_GET;
                }
                if(recv_buf[0] != 0x55)
                    bootloader_flag |= 1;
            }
            break;
        case HEADER_GET:
            data_len = recv_buf[DATA_LEN+1] << 8 | recv_buf[DATA_LEN]; //LSB first
            if(recv_counter == (DATA_BEGIN + data_len + 1 + 1)) //checksum + end of frame
            {
                if(data_len == 15) // address + size + version[3] + checksum
                {
                    update_info.address  = SerialDataSort(4,(uint8_t*)&recv_buf[DATA_BEGIN],MSB_FIRST);
                    update_info.size     = SerialDataSort(4,(uint8_t*)&recv_buf[DATA_BEGIN+4],MSB_FIRST);
                    for(i=0; i<3; i++)
                        update_info.version[i] = SerialDataSort(3,(uint8_t*)&recv_buf[DATA_BEGIN+8],MSB_FIRST);
                    update_info.checksum = SerialDataSort(4,(uint8_t*)&recv_buf[DATA_BEGIN+11],MSB_FIRST);
                }
                else
                    bootloader_flag |= 1;

                if(update_info.size > 0x20000) // not larger than a sector 128k
                    bootloader_flag |= 1;

                if (update_info.address != FLASH_APP_ADDR) //0x802_0000 Bank 1 Sector 1
                    bootloader_flag |= 1;
                bootloader_program_state = TRANSMIT;
            }
            break;
        case TRANSMIT:
            frame_checksum = checksum(data_len, (uint8_t *)&recv_buf[DATA_BEGIN]);
            if(frame_checksum != recv_buf[DATA_BEGIN + data_len])
                bootloader_flag |= 1;
            if(bootloader_flag != 1)
                positive_response();
            else
                negative_response();
            bootloader_program_state = CLEAR_VAR;
            break;
        case UPDATE_DONE:
            break;
        default:
            break;
        }
    }
}

// volatile uint32_t debug_I;
void bootloader_main(void)
{
    uint32_t TimeNow;
    TimeNow = __HAL_TIM_GET_COUNTER(&htim2);
    bootloader_state = IDLE_STATE;
    while(1)
    {
        switch (bootloader_state)
        {
        case IDLE_STATE:
            // print("timecounter: %ld", (TimeNow - __HAL_TIM_GET_COUNTER(&htim2)));
            if(time_gap_1ms_base(TimeNow,5000))
            {
                bootloader_state = JUMP_STATE;
            }
            if(recv_counter > 0)
                bootloader_state = FLASH_PROGRAM_STATE;
            break;

        case FLASH_PROGRAM_STATE:     
            flash_program();
            bootloader_state = JUMP_STATE;
            break;

        case JUMP_STATE:
            // print("Timesup");
            /*This address should be MSP, aka main stack pointer, the fisrt of vector table*/
            if(*(uint32_t*)FLASH_APP_ADDR != 0xFFFFFFFF)
            {
                HAL_RCC_DeInit();
                HAL_DeInit();
                __set_MSP(*(uint32_t*)FLASH_APP_ADDR);
                jump_to_user_app();
            }
            else
            {
                print("no user app found");
                while(1)
                    ;
            }
            break;
        
        default:
            break;
        }
    }

    
}

typedef void (*pFunction)(void);
static void jump_to_user_app(void)
{
     /****this does work****/
	// void (*user_app_reset_handler)(void) = (void*)(*((uint32_t *)(FLASH_APP_ADDR+4))); //0x08020000 + 4 (reset handler)  
	// user_app_reset_handler();
    /**************************/
    volatile uint32_t JumpAddress;
    pFunction Jump_To_Application;
    JumpAddress = *(volatile uint32_t*)(FLASH_APP_ADDR + 4); 
    Jump_To_Application = (pFunction)JumpAddress;
    Jump_To_Application();

}

static void print(char *msg, ...)
{
	char buff[250];
	va_list args;
	va_start(args, msg);
	vsprintf(buff, msg, args);

	for (int i = 0; i < strlen(buff); i++)
	{
        HAL_UART_Transmit(&huart1, (uint8_t*)&buff[i], 1, 1000);
	}

    while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);
		;
}



/**
 * @brief   This function erases the memory.
 * @param   sector: the sector to be erased.
 * @param   numberofsector: number of sectors to be erased
 * @return  status: Report about the success of the erasing.
 */
Flash_Status flash_erase_sector(uint8_t sector, uint8_t numberofsector)
{
    Flash_Status status = FLASH_OK;
    FLASH_EraseInitTypeDef erase;
    erase.Banks = FLASH_BANK_1; //currently is only using Bank1
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.NbSectors = numberofsector;
    erase.Sector = sector;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    uint32_t errorcode;
    HAL_FLASH_Unlock();
    if(HAL_FLASHEx_Erase(&erase, &errorcode) != HAL_OK)
    {
        status = FLASH_ERROR;
    }
    HAL_FLASH_Lock();
    return status;
}


/**
 * @brief This function writes flash 
 * @param address: first address to be written to
 * @param *data: array of data that we want to write
 * @param length: 
 *         This parameter shall be aligned to the Flash word:
 *          - 256 bits for STM32H74x/5X devices (8x 32bits words)
 * @return report success of writing
*/
Flash_Status flash_write (uint32_t address, uint32_t* data, uint32_t length)
{
    Flash_Status status = FLASH_OK;
    HAL_FLASH_Unlock();
    if((address+(length*4)) >= FLASH_APP_END_ADDRESS) 
    {
        status = FLASH_ERROR_SIZE;
    }
    else
    {
        for(uint32_t i = 0; (i < length) && (status == FLASH_OK); i++)
        {
            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, address, data[i]) != HAL_OK)
            {
                status |= FLASH_ERROR_WRITE;
            }
            address += 4;
        }
    }
    HAL_FLASH_Lock();
    return status;
}