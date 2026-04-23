#ifndef __UI_H
#define __UI_H		
#include "stm32f4xx.h"

void Lcd_bootup_scrolling(void);//滚动动画
void UI_Scroll(const void *row1[], uint8_t row1_len, const void *row2[], uint8_t row2_len, uint8_t dir, uint16_t interval, uint16_t speed, uint32_t time_ms);
void APP_init(void); //UI初始化

void APP(u16 ms); //APP状态机

// 更新数据,20ms执行一次
void APP_data_update(void);

// 显示信息,2s执行一次
void APP_show_info(void);

// 手动控制的函数
void APP_control(void);


#endif  
	 
	 


