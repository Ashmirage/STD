#include "stm32f4xx.h"                  // Device header
#include "CountSensor.h"

volatile uint16_t CountSensor_Count;				//全局变量，用于计数

/**
  * 函    数：计数传感器初始化
  * 参    数：无
  * 返 回 值：无
  */
void CountSensor_Init(void)
{			
	
}

/**
  * 函    数：获取计数传感器的计数值
  * 参    数：无
  * 返 回 值：计数值，范围：0~65535
  */
uint16_t CountSensor_Get(void)
{
	return CountSensor_Count;
}

/**
  * 函    数：对射式红外传感器中断回调函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数将会在EXTI_All_callback里面被调用
  *          
  */
void CountSensor_callback(void)
{
	/*如果出现数据乱跳的现象，可再次判断引脚电平，以避免抖动*/
	if (HAL_GPIO_ReadPin(COUNT_SENSOR_PORT, COUNT_SENSOR_PIN) == 0)
	{
		CountSensor_Count ++;					//计数值自增一次
	}
}
