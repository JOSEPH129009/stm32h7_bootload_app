
#include "usart.h"
#include "tim.h"
#include "main.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "bootloader.h"
#include "variable.h"
#include "constant.h"
#include "bit_field.h"

static void print(char *msg, ...);
static void jump_to_user_app(void);
void header_info_get(void);
void update_firmware(void);
void update_done(void);
frame_ID_function_t do_frame_ID_stuff[]={
    NULL,
    header_info_get, //1
    NULL,
    NULL,
    update_firmware, //4
    update_done,  //5
};

enum{
    LSB_FIRST,
    MSB_FIRST
};

/**
 * @brief   This function sort serial data for MSB_FIRST & LSB_FIRST on little endian machine.
 * @param   *dest: data pointer for sorted value
 * @param   *src: data pointer to be sorted
 * @param   len: length.
 * @param   endianness: LSB_FIRST or MSB_FIRST
 */
void serial_sort_function(uint8_t *dest,const uint8_t* src, uint8_t len, uint32_t endianness)
{
    int i;
    if(endianness == MSB_FIRST)
    {
        for(i=0; i<len; i++)
            dest[len-i-1] = src[i];
    }
    else
    {
        for(i=0; i<len; i++)
            dest[i] = src[i];
    }
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

typedef struct firmware_update{
    uint32_t address;
    uint32_t size;
    uint8_t version[3];
    uint32_t checksum;
}firmware_update_t;

enum{
    MAIN_STATE_IDLE,
    MAIN_STATE_FLASH_PROGRAM,
    MAIN_STATE_JUMP
};
enum{
    SUB_STATE_CLEAR_VAR,
    SUB_STATE_FRAME_ID_CHECK,
    SUB_STATE_DO_FRAME_ID_STUFF,
    SUB_STATE_TRANSMIT,
};

void negative_response(uint32_t error_flag)
{
    transmit_buf[0] = BL_START_OF_FRAME;
    transmit_buf[1] = BL_NACK_FRAME;
    transmit_buf[2] = 4; //LSB
    transmit_buf[3] = 0; //MSB
    transmit_buf[4] = (uint8_t)error_flag;
    transmit_buf[5] = (uint8_t)(error_flag>>8);
    transmit_buf[6] = (uint8_t)(error_flag>>16);
    transmit_buf[7] = (uint8_t)(error_flag>>24);
    transmit_buf[8] = checksum(4,&transmit_buf[4]); //checksum
    transmit_buf[9] = BL_END_OF_FRAME; 
    transmit_data_len = 10;
}
void positive_response(void)
{
    transmit_buf[0] = BL_START_OF_FRAME;
    transmit_buf[1] = BL_ACK_FRAME;
    transmit_buf[2] = 0; //length LSB
    transmit_buf[3] = 0; //lenght MSB
    transmit_buf[4] = 0; //checksum
    transmit_buf[5] = BL_END_OF_FRAME; 
    transmit_data_len = 6;
}

#define DATA_LEN   2
#define DATA_BEGIN 4
uint16_t recv_payload_len = 0;
uint8_t frame_checksum = 0;
int i = 0;
firmware_update_t update_info = {0};
void flash_program(void)
{
    switch (bootloader_state.Sub_State)
    {
    case SUB_STATE_CLEAR_VAR:
        bootloader_error_flag = 0; //clear flag
        bootloader_flag = bit_off(bootloader_flag,FirstByteRecv);
        recv_uart_counter = 0;
        recv_payload_len = 0;
        transmit_data_len = 0;
        memset((void *)recv_buf, 0xFF, sizeof(frame_format_t));
        switch_sub_state(&bootloader_state,SUB_STATE_FRAME_ID_CHECK);
        break;
    case SUB_STATE_FRAME_ID_CHECK:
        if(bootloader_flag & FirstByteRecv)
        {
            if(!time_gap_1ms_base(serial_port_timer,2000))
            {
                if(recv_uart_counter >= 4)
                {
                    recv_payload_len = recv_buf[DATA_LEN+1] << 8 | recv_buf[DATA_LEN]; //LSB first
                    if(recv_uart_counter == (DATA_BEGIN + recv_payload_len + 1 + 1)) //checksum + end of frame
                    {
                        switch_sub_state(&bootloader_state,SUB_STATE_DO_FRAME_ID_STUFF);
                    }

                    if(recv_buf[0] != 0x55)
                    {
                        bootloader_error_flag = bit_on(bootloader_error_flag, FrameError);
                        negative_response(bootloader_error_flag);
                        switch_sub_state(&bootloader_state,SUB_STATE_TRANSMIT);
                    }
                }
            }
            else 
            {
                bootloader_error_flag = bit_on(bootloader_error_flag, TimeOut);
                switch_sub_state(&bootloader_state,SUB_STATE_TRANSMIT);
            }
        }
        break;
    case SUB_STATE_DO_FRAME_ID_STUFF:
        frame_checksum = checksum(recv_payload_len, (uint8_t *)&recv_buf[DATA_BEGIN]);
        if(frame_checksum != recv_buf[DATA_BEGIN + recv_payload_len]) //checksum
            bootloader_error_flag = bit_on(bootloader_error_flag, FrameError);

        if(0xFF != recv_buf[DATA_BEGIN + recv_payload_len +1]) //BL_END_OF_FRAME
            bootloader_error_flag = bit_on(bootloader_error_flag, FrameError);

        recv_buf[DATA_BEGIN + recv_payload_len] = 0xFF;//checksum update to flash for the payload less than 256

        if(bootloader_error_flag == 0)
            do_frame_ID_stuff[recv_buf[1]]();

        if(bootloader_error_flag == 0)
            positive_response();
        else
            negative_response(bootloader_error_flag);

        switch_sub_state(&bootloader_state,SUB_STATE_TRANSMIT);
        break;
    case SUB_STATE_TRANSMIT:
        while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX)
            ;
        HAL_UART_Transmit(&huart1,&transmit_buf[0],transmit_data_len,1000);
        if(bootloader_flag & UpdateDone)
            bootloader_flag = bit_on(bootloader_flag,UpdateDoneReply);
        LED0_Toggle;
        switch_sub_state(&bootloader_state,SUB_STATE_CLEAR_VAR);             
        break;
    default:
        break;
    }
}


// volatile uint32_t debug_I;
void bootloader_main(void)
{
    uint32_t TimeNow = 0;
    uint32_t light_timer = 0;
    TimeNow = Get_Timer_1_ms_Base;
    light_timer = Get_Timer_1_ms_Base;
    bootloader_state.Main_State = MAIN_STATE_IDLE;
    while(1)
    {
        switch (bootloader_state.Main_State)
        {
        case MAIN_STATE_IDLE:
            // print("timecounter: %ld", (TimeNow - __HAL_TIM_GET_COUNTER(&htim2)));
            if(time_gap_1ms_base(TimeNow,5000))
            {
                switch_main_state(MAIN_STATE_JUMP, 0);
            }

            if(time_gap_1ms_base(light_timer,500))
            {
                LED0_Toggle;
                light_timer = Get_Timer_1_ms_Base;
            }

            if(recv_uart_counter > 0)
                switch_main_state(MAIN_STATE_FLASH_PROGRAM, SUB_STATE_FRAME_ID_CHECK);
            break;
        case MAIN_STATE_FLASH_PROGRAM:     
            if(!(bootloader_flag&UpdateDoneReply))
            {
                flash_program();
            }
            else
            {
                switch_main_state(MAIN_STATE_JUMP, 0);
            }
            break;
        case MAIN_STATE_JUMP:
            // print("Timesup");
            /*This address should be MSP, aka main stack pointer, the fisrt of vector table*/
            if(*(volatile uint32_t*)FLASH_APP_ADDR != 0xFFFFFFFF)
            {
                HAL_RCC_DeInit();
                HAL_DeInit();
                __set_MSP(*(uint32_t*)FLASH_APP_ADDR);
                jump_to_user_app();
            }
            else
            {
                // print("no user app found");
                LED1_On;
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
     /****this does work with -O2****/
	void (*user_app_reset_handler)(void) = (void*)(*((uint32_t *)(FLASH_APP_ADDR+4))); //0x08020000 + 4 (reset handler)  
	user_app_reset_handler();
    /*******this works with -O0**********/
    // volatile uint32_t JumpAddress;
    // pFunction Jump_To_Application;
    // JumpAddress = *(volatile uint32_t*)(FLASH_APP_ADDR + 4); 
    // Jump_To_Application = (pFunction)JumpAddress;
    // Jump_To_Application();

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
 * @brief   This function erases the memory in BANK 1.
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
    if(HAL_OK != HAL_FLASHEx_Erase(&erase, &errorcode))
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
 *         max value: 256 btye
 *         This parameter shall be aligned to a Flash word:
 *         A flash word - 256 bits for STM32H74x/5X devices (8x 32bits words =32 bytes)
 *         if not aligned to a flash word, it automatically fills up with 0xFF
 * @return report success of writing
*/
Flash_Status flash_write (uint32_t address, uint32_t* data, uint32_t length)
{
    Flash_Status status = FLASH_OK;
    uint32_t remainder = 0;
    remainder = length%8;
    HAL_FLASH_Unlock();
    if((length > 256 || (address + length + remainder) >= FLASH_APP_END_ADDRESS)) 
    {
        status = FLASH_ERROR_SIZE;
    }
    else
    {
        for(uint32_t i = 0; (i < length/4) && (status == FLASH_OK);) /*i increment 1 = 4 bytes*/
        {
            if(HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, address, (uint32_t)&data[i]))
            {
                status |= FLASH_ERROR_WRITE;
            }
            address += 32; //flash word 256bit / 8bit
            i += 8; // a flash word 32byte / 4 byte(uint32_t)
        }
    }
    HAL_FLASH_Lock();
    return status;
}


void header_info_get(void)
{                
    if(recv_payload_len == 15) // address + size + version[3] + checksum
    {
        serial_sort_function((uint8_t*)&update_info.address,
                                        (uint8_t*)&recv_buf[DATA_BEGIN]  ,4,LSB_FIRST);
        serial_sort_function((uint8_t*)&update_info.size,
                                        (uint8_t*)&recv_buf[DATA_BEGIN+4],4,LSB_FIRST);
        serial_sort_function((uint8_t*)&update_info.version[0],
                                        (uint8_t*)&recv_buf[DATA_BEGIN+8],3,MSB_FIRST);
        serial_sort_function((uint8_t*)&update_info.checksum,
                                        (uint8_t*)&recv_buf[DATA_BEGIN+11],4,LSB_FIRST);
    }
    else
        bootloader_error_flag = bit_on(bootloader_error_flag, WrongUpdateInfo);

    if(update_info.size > FLASH_SECTOR_SIZE) // not larger than a sector 128k
        bootloader_error_flag = bit_on(bootloader_error_flag, WrongUpdateInfo);

    if (update_info.address != FLASH_APP_ADDR) //0x802_0000 Bank 1 Sector 1
        bootloader_error_flag = bit_on(bootloader_error_flag, WrongUpdateInfo);
    
    if(bootloader_error_flag == 0)
    {
        if(FLASH_OK != flash_erase_sector(1,1))
            bootloader_error_flag = bit_on(bootloader_error_flag, WrongUpdateInfo);
        else
            bootloader_flag = bit_on(bootloader_flag, UpdateInfoGet);
    }
}

void update_firmware(void)
{
    if(bootloader_flag & UpdateInfoGet)
    {
        if(FLASH_OK != flash_write(update_info.address+address_counter,
                                    (uint32_t*)&recv_buf[DATA_BEGIN],recv_payload_len))
        {
            bootloader_error_flag = bit_on(bootloader_error_flag, UpdateError);
        }
        address_counter = address_counter + recv_payload_len;
    }
    else
    {
        bootloader_error_flag = bit_on(bootloader_error_flag, UpdateError);
    }
}

void update_done(void)
{
    if(bootloader_error_flag == 0)
        bootloader_flag = bit_on(bootloader_flag,UpdateDone);
}