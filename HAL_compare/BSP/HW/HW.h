#ifndef __HW_H
#define	__HW_H
#include "stm32f4xx.h"
#include "delay_us.h"
#include "math.h"

/*****************辰哥单片机设计******************
											STM32
 * 文件			:	光电红外传感器h文件                   
 * 版本			: V1.0
 * 日期			: 2024.8.12
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码								
 * IP账号		:	辰哥单片机设计（同BILIBILI|抖音|快手|小红书|CSDN|公众号|视频号等）
 * 作者			:	辰哥
 * 工作室		: 异方辰电子工作室
 * 讲解视频	:	https://www.bilibili.com/video/BV1Rf421v7Mx/?share_source=copy_web
 * 官方网站	:	www.yfcdz.cn

**********************BEGIN***********************/	

/***************根据自己需求更改****************/
// HW GPIO宏定义

#define 	HW_GPIO_PORT							GPIOB
#define 	HW_GPIO_PIN								GPIO_PIN_13			

/*********************END**********************/


void HW_Init(void);
uint16_t HW_GetData(void);

#endif /* __ADC_H */

