#ifndef _DELAY_US_H_
#define _DELAY_US_H_

#include "stm32f4xx_hal.h"  // 根据你的芯片选择头文件

void DWT_Init(void);
void delay_us(uint32_t us);
/* 获取微秒级时间戳（可累积超过 CYCCNT 溢出） */
uint64_t get_us_timestamp(void);
#endif

