#ifndef __AD_H
#define __AD_H

#include <stdint.h>

extern volatile uint16_t AD_Buf[3];

/* 公共必抄：初始化 ADC3 + DMA，固定扫描 PF7、PA0、PA3 三路。 */
void AD_Init(void);

/* 可选：电位器，PF7 / ADC3_CH5。 */
uint16_t AD_GetPotRaw(void);

/* 可选：热敏电阻，PA0 / ADC3_CH0。 */
uint16_t AD_GetTempRaw(void);
int16_t AD_GetTempCelsius(void);

/* 可选：光敏电阻，PA3 / ADC3_CH3。比赛只用光敏时抄这两个接口。 */
uint16_t AD_GetLdrRaw(void);
uint16_t AD_GetLdrLux(void);

#endif
