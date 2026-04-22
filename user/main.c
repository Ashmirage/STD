#include "delay.h"
#include "scheduler.h"

int main(void){
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


