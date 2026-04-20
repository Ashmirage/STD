#include "stm32f4xx.h"
#include "delay.h"
#include "lcd.h"
#include "scheduler.h"


int main(void){
	GPIO_InitTypeDef gpio_st;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	gpio_st.GPIO_Mode = GPIO_Mode_OUT;
	gpio_st.GPIO_OType = GPIO_OType_PP;
	gpio_st.GPIO_Pin = GPIO_Pin_10;
	gpio_st.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_st.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF,&gpio_st);
	delay_init(168);
	Scheduler_init();
	Hardware_init();
	while(1){
		Scheduler_run();
//		GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_RESET);
//		delay_ms(500);
//		GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_SET);
//		delay_ms(500);
	}

}


