#ifndef __UI_H
#define __UI_H		
#include "stm32f4xx.h"

void APP_init(void); //UI初始化

void APP(u16 ms); //APP状态机

// 更新数据,20ms执行一次
void APP_data_update(void);

// 显示信息,2s执行一次
void APP_show_info(void);


// 业务状态枚举
typedef enum{
	APP_CHANGE_PASSWORD, //修改密码状态
	APP_LOCKED, //未解锁状态
	APP_PRE_UNLOCKED, //输入密码的状态
	APP_UNLOCKED, //解锁状态
}APP_STATUS;

extern volatile APP_STATUS app_status;// 定义业务状态
#endif  
	 
	 
