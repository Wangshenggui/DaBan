/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct
{
    uint8_t SysBeng;//ˮ�ùر�״̬
}RunGuidance_Structure;
extern RunGuidance_Structure RunGuidance_Struct;
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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RTK_LED_Pin GPIO_PIN_13
#define RTK_LED_GPIO_Port GPIOC
#define GPS_RES_Pin GPIO_PIN_2
#define GPS_RES_GPIO_Port GPIOC
#define Key_Pin GPIO_PIN_0
#define Key_GPIO_Port GPIOA
#define GPS_TX_Pin GPIO_PIN_2
#define GPS_TX_GPIO_Port GPIOA
#define GPS_RX_Pin GPIO_PIN_3
#define GPS_RX_GPIO_Port GPIOA
#define LiquidLevel_Pin GPIO_PIN_6
#define LiquidLevel_GPIO_Port GPIOA
#define Pump_Pin GPIO_PIN_0
#define Pump_GPIO_Port GPIOB
#define SlaveRS485_Tx_Pin GPIO_PIN_10
#define SlaveRS485_Tx_GPIO_Port GPIOB
#define SlaveRS485_Rx_Pin GPIO_PIN_11
#define SlaveRS485_Rx_GPIO_Port GPIOB
#define SlaveRS485_RE_Pin GPIO_PIN_12
#define SlaveRS485_RE_GPIO_Port GPIOB
#define RadioM0_Pin GPIO_PIN_9
#define RadioM0_GPIO_Port GPIOC
#define RadioM1_Pin GPIO_PIN_8
#define RadioM1_GPIO_Port GPIOA
#define Radio_TX_Pin GPIO_PIN_9
#define Radio_TX_GPIO_Port GPIOA
#define Radio_RX_Pin GPIO_PIN_10
#define Radio_RX_GPIO_Port GPIOA
#define RTK_TX_Pin GPIO_PIN_10
#define RTK_TX_GPIO_Port GPIOC
#define RTK_RX_Pin GPIO_PIN_11
#define RTK_RX_GPIO_Port GPIOC
#define led_Pin GPIO_PIN_9
#define led_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
