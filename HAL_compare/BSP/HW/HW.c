#include "HW.h"

/*****************辰哥单片机设计******************
											STM32
 * 文件			:	光电红外传感器c文件                   
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

void HW_Init(void)
{
	
}

// 获取结果
uint16_t HW_GetData(void)
{
	uint16_t tempData;
	tempData = !HAL_GPIO_ReadPin(HW_GPIO_PORT, HW_GPIO_PIN);
	return tempData;
}



