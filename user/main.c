#include "delay.h"
#include "scheduler.h"


int main(void){
	delay_init(168); // 晶振和延时函数初始化
	Scheduler_init();                      // 任务调度器初始化
	Hardware_init();     // 模块初始化
	while(1){
		Scheduler_run();    // 任务调度器
	}

}


