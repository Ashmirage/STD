# AD 模块
- 电位器：原始值
- 热敏电阻：原始值、温度
- 光敏电阻：原始值、光照

核心文件：

- `driver/bsp/AD.h`
- `driver/bsp/AD.c`

## 默认引脚

| 模块 | 引脚 | 通道 |
|---|---|---|
| Pot | `PF7` | `ADC3_IN5` |  电位器从RV1那个跳线帽那里引出来
| Temp | `PA0` | `ADC3_IN0` |
| LDR | `PA3` | `ADC3_IN3` |



## 现在只保留的接口

```c
void AD_Init(void);

uint16_t AD_GetPotRaw(void);

uint16_t AD_GetTempRaw(void);
int16_t  AD_GetTempCelsius(void);

uint16_t AD_GetLdrRaw(void);
uint16_t AD_GetLdrLux(void);
```


## 最简单用法

```c
#include "AD.h"

AD_Init();

Send_printf("pot=%d temp_raw=%d temp=%dC ldr_raw=%d lux=%d\r\n",
            AD_GetPotRaw(),
            AD_GetTempRaw(),
            AD_GetTempCelsius(),
            AD_GetLdrRaw(),
            AD_GetLdrLux());
```
## 接线说明补充

### 1. 电位器 `PF7`

- 如果你用探索者板子上的电位器，直接按板上原理图走，当前代码默认从 `RV1` 那一路把模拟量引到 `PF7`

### 2. 热敏电阻 `PA0`

### 3. 光敏电阻 `PA3`

## 当前工程接入位置

当前工程已经在 `Hardware_init()` 里初始化：

```c
void Hardware_init(void)
{
    uart_init(115200);
    // 其他初始化...

    AD_Init();
}
```

当前工程里已经留了串口打印示例，直接取消注释就能看值：

```c
Send_printf("pot=%d temp_raw=%d temp=%dC ldr_raw=%d lux=%d\r\n",
            AD_GetPotRaw(),
            AD_GetTempRaw(),
            AD_GetTempCelsius(),
            AD_GetLdrRaw(),
            AD_GetLdrLux());
```


## 比赛直接抄的完整代码



根据用到的模块,选择部分代码抄写

### AD.c

```c
#include "stm32f4xx.h"
#include "AD.h"
#include <math.h>

/* ===== 公共必抄代码开始：ADC3 + DMA 连续扫描基础配置 =====
 * 当前固定三路同时采样：
 * AD_Buf[0] = PF7 电位器
 * AD_Buf[1] = PA0 热敏电阻
 * AD_Buf[2] = PA3 光敏电阻
 *
 * 如果比赛只用光敏电阻，最省事的抄法：
 * 1. 公共必抄代码整段保留
 * 2. 下面只抄“光敏电阻可选代码”
 * 3. 电位器、热敏电阻两个可选函数可以不抄、不调用
 */
static const float vref = 3.3f;
static const float full = 4095.0f;
static const float ntc_r = 10000.0f;
static const float ntc_b = 3950.0f;
static const float ntc_t = 298.15f;
static const float ldr_r = 10000.0f;

volatile uint16_t AD_Buf[3];

static uint8_t ok = 0U;

/* 公共必抄：把三个 ADC 引脚配置成模拟输入。 */
static void gpio_cfg(void)
{
    GPIO_InitTypeDef gpio;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOA, ENABLE);

    gpio.GPIO_Mode = GPIO_Mode_AN;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;

    gpio.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOF, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3;
    GPIO_Init(GPIOA, &gpio);
}

/* 公共必抄：DMA 循环搬运 ADC3 的 3 路采样值到 AD_Buf[]。 */
static void dma_cfg(void)
{
    DMA_InitTypeDef dma;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    DMA_Cmd(DMA2_Stream0, DISABLE);
    while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE)
    {
    }

    DMA_DeInit(DMA2_Stream0);

    dma.DMA_Channel = DMA_Channel_2;
    dma.DMA_PeripheralBaseAddr = (uint32_t)&ADC3->DR;
    dma.DMA_Memory0BaseAddr = (uint32_t)AD_Buf;
    dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
    dma.DMA_BufferSize = 3;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dma.DMA_Mode = DMA_Mode_Circular;
    dma.DMA_Priority = DMA_Priority_Low;
    dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
    dma.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &dma);
}

/* 公共必抄：ADC3 固定扫描 3 路，顺序必须和 AD_Buf[] 下标一致。 */
static void adc_cfg(void)
{
    ADC_CommonInitTypeDef com;
    ADC_InitTypeDef adc;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

    ADC_CommonStructInit(&com);
    com.ADC_Mode = ADC_Mode_Independent;
    com.ADC_Prescaler = ADC_Prescaler_Div4;
    com.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    com.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&com);

    ADC_StructInit(&adc);
    adc.ADC_Resolution = ADC_Resolution_12b;
    adc.ADC_ScanConvMode = ENABLE;
    adc.ADC_ContinuousConvMode = ENABLE;
    adc.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    adc.ADC_DataAlign = ADC_DataAlign_Right;
    adc.ADC_NbrOfConversion = 3;
    ADC_Init(ADC3, &adc);

    ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_144Cycles);
    ADC_RegularChannelConfig(ADC3, ADC_Channel_0, 2, ADC_SampleTime_144Cycles);
    ADC_RegularChannelConfig(ADC3, ADC_Channel_3, 3, ADC_SampleTime_144Cycles);

    ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);
    ADC_DMACmd(ADC3, ENABLE);
    ADC_Cmd(ADC3, ENABLE);
}

/* 公共必抄：只初始化一次，避免重复打开 DMA 和 ADC。 */
void AD_Init(void)
{
    if (ok != 0U)
    {
        return;
    }

    gpio_cfg();
    dma_cfg();
    adc_cfg();

    DMA_Cmd(DMA2_Stream0, ENABLE);
    ADC_SoftwareStartConv(ADC3);

    ok = 1U;
}

static uint16_t read(uint8_t i)
{
    if (i >= 3)
    {
        return 0U;
    }

    return AD_Buf[i];
}
/* ===== 公共必抄代码结束 ===== */

/* ===== 电位器可选代码：不用电位器时，这个函数可以不抄 ===== */
uint16_t AD_GetPotRaw(void)
{
    return read(0);
}

/* ===== 热敏电阻可选代码：不用温度时，下面两个函数可以不抄 ===== */
uint16_t AD_GetTempRaw(void)
{
    return read(1);
}

/* 热敏电阻：返回摄氏温度整数值。 */
int16_t AD_GetTempCelsius(void)
{
    float v = ((float)AD_GetTempRaw()) * (vref / full);
    float r;
    float inv_t;
    float t;

    if (v < 0.01f)
    {
        v = 0.01f;
    }

    if (v > (vref - 0.01f))
    {
        v = vref - 0.01f;
    }

    r = ntc_r * v / (vref - v);
    if (r < 10.0f)
    {
        r = 10.0f;
    }

    inv_t = (1.0f / ntc_t)
        + (1.0f / ntc_b) * (float)log((double)(r / ntc_r));
    t = (1.0f / inv_t) - 273.15f;

    if (t >= 0.0f)
    {
        return (int16_t)(t + 0.5f);
    }

    return (int16_t)(t - 0.5f);
}

/* ===== 光敏电阻可选代码：比赛只用光敏时，抄公共代码 + 下面两个函数 ===== */
uint16_t AD_GetLdrRaw(void)
{
    return read(2);
}

/* 光敏电阻：返回估算 Lux 整数值。 */
uint16_t AD_GetLdrLux(void)
{
    float v = ((float)AD_GetLdrRaw()) * (vref / full);
    float r;
    float lux;

    if (v < 0.01f)
    {
        v = 0.01f;
    }

    if (v > (vref - 0.01f))
    {
        v = vref - 0.01f;
    }

    r = v / (vref - v) * ldr_r;
    lux = 40000.0f * (float)pow((double)r, -0.6021);

    if (lux < 0.0f)
    {
        lux = 0.0f;
    }

    if (lux > 65535.0f)
    {
        lux = 65535.0f;
    }

    return (uint16_t)(lux + 0.5f);
}

```

### AD.h

```c
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

```

### Pot.h

```c
#ifndef __POT_H
#define __POT_H

#include <stdint.h>

/* 兼容旧接口，只保留电位器原始值读取。 */
void Pot_Init(void);

uint16_t Pot_GetData(void);

#endif
```

### Pot.c

```c
#include "Pot.h"
#include "AD.h"

void Pot_Init(void)
{
    AD_Init();
}

/* 电位器只保留原始 AD 值。 */
uint16_t Pot_GetData(void)
{
    return AD_GetPotRaw();
}
```
