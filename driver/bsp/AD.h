#ifndef __AD_H
#define __AD_H

#include <stdint.h>

/* 默认把三个模拟模块统一挂到 ADC3 + DMA 上连续扫描。 */
/* 默认引脚分配：Pot -> PF7，Temp -> PA0，LDR -> PA3。 */
#define AD_POT_EN    1
#define AD_TMP_EN    1
#define AD_LDR_EN    1

#if ((AD_POT_EN != 0) && (AD_POT_EN != 1))
#error "AD_POT_EN must be 0 or 1"
#endif

#if ((AD_TMP_EN != 0) && (AD_TMP_EN != 1))
#error "AD_TMP_EN must be 0 or 1"
#endif

#if ((AD_LDR_EN != 0) && (AD_LDR_EN != 1))
#error "AD_LDR_EN must be 0 or 1"
#endif

#define AD_N         (AD_POT_EN + AD_TMP_EN + AD_LDR_EN)

#if (AD_N == 0)
#error "At least one ADC channel must be enabled"
#endif

#define AD_POT_I     0
#define AD_TMP_I     (AD_POT_EN)
#define AD_LDR_I     (AD_POT_EN + AD_TMP_EN)

#define AD_VREF      3.3f
#define AD_MAX       4095.0f
#define TMP_R        10000.0f
#define TMP_B        3950.0f
#define TMP_R0       10000.0f
#define TMP_T0       298.15f

#define LDR_R        10000.0f

extern volatile uint16_t AD_Buf[AD_N];

/* 初始化 ADC3 + DMA，开始循环采样。 */
void AD_Init(void);

/* 电位器：只保留原始值。 */
uint16_t AD_GetPotRaw(void);

/* 热敏：保留原始值和温度值。 */
uint16_t AD_GetTempRaw(void);
int16_t AD_GetTempCelsius(void);

/* 光敏：保留原始值和光照值。 */
uint16_t AD_GetLdrRaw(void);
uint16_t AD_GetLdrLux(void);

#endif
