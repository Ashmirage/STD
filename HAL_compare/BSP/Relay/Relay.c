#include "stm32f4xx.h"

#define RELAY_GPIO_PORT GPIOC
#define RELAY_GPIO_PIN GPIO_PIN_4

// 继电器初始化,默认NO COM之间断开
void Relay_init(void)
{
	HAL_GPIO_WritePin(RELAY_GPIO_PORT,RELAY_GPIO_PIN,GPIO_PIN_RESET);  //高电平有效,默认失效
}


// 接通
void Relay_on(void)
{
	HAL_GPIO_WritePin(RELAY_GPIO_PORT,RELAY_GPIO_PIN,GPIO_PIN_SET); 
}

//断开
void Relay_off(void)
{
	HAL_GPIO_WritePin(RELAY_GPIO_PORT,RELAY_GPIO_PIN,GPIO_PIN_RESET); 
}

// 改变继电器状态
void Relay_status(uint8_t status)
{
	if(status == 1)
	{
		Relay_on();
	}else
	{
		Relay_off();
	}
}




