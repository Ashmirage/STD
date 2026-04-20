#include "stm32f4xx.h"
#include "CountSensor.h"

// 所有的EXTI中断都写在这
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	// 对射式传感器触发
    if (GPIO_Pin == COUNT_SENSOR_PIN)
    {
		CountSensor_callback();
        // PC13 中断触发后的处理代码
    }

    if (GPIO_Pin == GPIO_PIN_0)
    {
        // PA0 中断触发后的处理代码
    }
}














