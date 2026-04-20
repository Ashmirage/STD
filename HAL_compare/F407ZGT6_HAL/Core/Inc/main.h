/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

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
#define row3_Pin GPIO_PIN_2
#define row3_GPIO_Port GPIOE
#define row2_Pin GPIO_PIN_4
#define row2_GPIO_Port GPIOE
#define row1_Pin GPIO_PIN_6
#define row1_GPIO_Port GPIOE
#define buzzer_Pin GPIO_PIN_8
#define buzzer_GPIO_Port GPIOF
#define LED_RED_Pin GPIO_PIN_9
#define LED_RED_GPIO_Port GPIOF
#define LED_GREEN_Pin GPIO_PIN_10
#define LED_GREEN_GPIO_Port GPIOF
#define col1_Pin GPIO_PIN_0
#define col1_GPIO_Port GPIOC
#define col2_Pin GPIO_PIN_1
#define col2_GPIO_Port GPIOC
#define col3_Pin GPIO_PIN_2
#define col3_GPIO_Port GPIOC
#define col4_Pin GPIO_PIN_3
#define col4_GPIO_Port GPIOC
#define motor_in1_Pin GPIO_PIN_4
#define motor_in1_GPIO_Port GPIOA
#define stepmotor_in1_Pin GPIO_PIN_5
#define stepmotor_in1_GPIO_Port GPIOA
#define motor_in2_Pin GPIO_PIN_6
#define motor_in2_GPIO_Port GPIOA
#define relay_Pin GPIO_PIN_4
#define relay_GPIO_Port GPIOC
#define stepmotor_in4_Pin GPIO_PIN_0
#define stepmotor_in4_GPIO_Port GPIOB
#define HW_Pin GPIO_PIN_13
#define HW_GPIO_Port GPIOB
#define Countsensor_Pin GPIO_PIN_14
#define Countsensor_GPIO_Port GPIOB
#define Countsensor_EXTI_IRQn EXTI15_10_IRQn
#define dht11_Pin GPIO_PIN_6
#define dht11_GPIO_Port GPIOC
#define stepmotor_in2_Pin GPIO_PIN_11
#define stepmotor_in2_GPIO_Port GPIOA
#define stepmotor_in3_Pin GPIO_PIN_12
#define stepmotor_in3_GPIO_Port GPIOA
#define IIC_SCL_Pin GPIO_PIN_8
#define IIC_SCL_GPIO_Port GPIOB
#define IIC_SDA_Pin GPIO_PIN_9
#define IIC_SDA_GPIO_Port GPIOB
#define row4_Pin GPIO_PIN_0
#define row4_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
