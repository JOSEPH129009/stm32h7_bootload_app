/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct OPERATION_STATE
{
	uint8_t Main_State;
	uint8_t Sub_State;
	uint8_t Ex_Sub_State;
}OP_STATE;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void switch_main_state(uint8_t main_status,uint8_t sub_status);
void switch_sub_state(OP_STATE* S, uint8_t D);
void switch_to_ex_sub_state(OP_STATE* S);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_5
#define LED1_GPIO_Port GPIOE
#define LED0_Pin GPIO_PIN_5
#define LED0_GPIO_Port GPIOB
#define FTDI_tx_Pin GPIO_PIN_6
#define FTDI_tx_GPIO_Port GPIOB
#define FTDI_rx_Pin GPIO_PIN_7
#define FTDI_rx_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define LED0_Off HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
#define LED0_On HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
#define LED0_Toggle HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
#define LED1_Off HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
#define LED1_On HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
