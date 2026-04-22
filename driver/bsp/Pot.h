#ifndef __POT_H
#define __POT_H

#include <stdint.h>

/* 初始化电位器 ADC 通道。当前使用 PA5 = ADC1_IN5。 */
void Pot_Init(void);

/* 读取电位器当前的 12 位 ADC 原始值，范围 0~4095。 */
uint16_t Pot_GetData(void);

/* 读取电位器当前电压，按 3.3V 参考电压换算。 */
float Pot_GetVoltage(void);

#endif
