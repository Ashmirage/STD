#ifndef __HCSR04_H
#define __HCSR04_H

#include <stdint.h>

/* 初始化超声波模块：默认 PE1=TRIG，PE3=ECHO，TIM5 计时 */
void HCSR04_Init(void);

/* 读取距离：成功返回 1，失败返回 0，距离单位 cm */
uint8_t HCSR04_Read(float *cm);

#endif
