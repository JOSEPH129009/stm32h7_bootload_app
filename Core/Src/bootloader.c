
#include "usart.h"
#include "tim.h"
#include "main.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "bootloader.h"

// volatile uint32_t debug_I;
void bootloader_main(void)
{
    uint32_t TimeNow;
    TimeNow = __HAL_TIM_GET_COUNTER(&htim2);
    while(1)
    {
        print("timecounter: %ld", (TimeNow - __HAL_TIM_GET_COUNTER(&htim2)));
        if(time_gap_1ms_base(TimeNow,5000))
        {
            print("Timesup");
            /*
             handle if no app found
            */
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
                while(1);
            }
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