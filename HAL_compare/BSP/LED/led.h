#ifndef __LED_H
#define __LED_H

#include "stm32f4xx.h"
void LED_ON(void);

void LED_OFF(void);


void LED_Toggle(void);



// LED专用时钟,在定时器中断函数中被调用
void LED_tick(void);

// 开启LED闪烁
void LED_flash_on(uint16_t ms);

void LED_flash_off(void);

uint8_t LED_get_status(void);
#endif


