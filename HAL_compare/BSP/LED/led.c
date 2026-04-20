#include "stm32f4xx.h"

#define LED_PORT    GPIOF
#define LED_PIN     GPIO_PIN_9


static uint8_t led_status = 0;

void LED_ON(void){
	HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_RESET);
	led_status = 1;
}

void LED_OFF(void){
	HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_SET);
	led_status = 0;
}


void LED_Toggle(void){
	if(HAL_GPIO_ReadPin(LED_PORT,LED_PIN) == GPIO_PIN_SET){
		HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_RESET);
	}else{
		HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_SET);
	}
	led_status = 1 - led_status;
}

static uint32_t led_tick = 0;
static uint8_t mode = 0;// 默认0非闪烁模式,1闪烁模式
static uint16_t led_ms = 500;//闪烁间隔,默认500ms


// LED专用时钟,在定时器中断函数中被调用
void LED_tick(void)
{
	led_tick++;
	if(led_tick >= led_ms)
	{
		led_tick = 0;
		if(mode == 1)
		{
			LED_Toggle();//到时间了,toggle一下
		}
	}
}

// 开启LED闪烁
void LED_flash_on(uint16_t ms)
{
	if(ms <= 100) //进行粗略限幅
	{
		ms = 100;
	}
	led_ms = ms;//设置LED闪烁的间隔时间
	mode = 1;//开启闪烁
}

void LED_flash_off(void)
{
	mode = 0;//关闭闪烁
}

uint8_t LED_get_status(void){
	return led_status;
}

