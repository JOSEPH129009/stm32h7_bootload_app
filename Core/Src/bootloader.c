
#include "usart.h"
#include "tim.h"
#include "main.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "bootloader.h"

static void print(char *msg, ...);
#define FLASH_APP_ADDR 0x08020000   //bank 1 sector 1 total of 128k

static void jump_to_user_app(void);

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
            HAL_RCC_DeInit();
            HAL_DeInit();
            __set_MSP(*(uint32_t*)FLASH_APP_ADDR);
            jump_to_user_app();

        }
        // else  print app not found
    }

}

typedef void (*pFunction)(void);
static void jump_to_user_app(void)
{
     /****this does work****/
	// void (*user_app_reset_handler)(void) = (void*)(*((uint32_t *)(FLASH_APP_ADDR+4))); //0x08020000 + 4 (reset handler)  
	// user_app_reset_handler();
    /**************************/
    uint32_t JumpAddress;
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