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

## 保留的宏

在 `driver/bsp/AD.h` 里保留了这三个宏，不用的模块直接关掉：

```c
#define AD_POT_EN    1
#define AD_TMP_EN    1
#define AD_LDR_EN    1
```

例如：

- 只用电位器：`1 0 0`
- 只用热敏：`0 1 0`
- 只用光敏：`0 0 1`
- 电位器 + 光敏：`1 0 1`

## 现在只保留的接口

```c
void AD_Init(void);

uint16_t AD_GetPotRaw(void);

uint16_t AD_GetTempRaw(void);
int16_t  AD_GetTempCelsius(void);

uint16_t AD_GetLdrRaw(void);
uint16_t AD_GetLdrLux(void);
```

同时保留：

```c
extern volatile uint16_t AD_Buf[AD_N];
```

- `AD_Buf[0] = Pot`
- `AD_Buf[1] = Temp`
- `AD_Buf[2] = LDR`

如果你关掉了一部分模块，下标会自动前移，所以业务代码更建议直接用上面的函数。

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

## 更改引脚/通道时要一起改什么

- 开关模块数量：改 `driver/bsp/AD.h`
  - `AD_POT_EN`
  - `AD_TMP_EN`
  - `AD_LDR_EN`
- 改电位器引脚：改 `driver/bsp/AD.c`
  - `POT_PORT`
  - `POT_CLK`
  - `POT_PIN`
  - `POT_CH`
- 改热敏引脚：改 `driver/bsp/AD.c`
  - `TMP_PORT`
  - `TMP_CLK`
  - `TMP_PIN`
  - `TMP_CH`
- 改光敏引脚：改 `driver/bsp/AD.c`
  - `LDR_PORT`
  - `LDR_CLK`
  - `LDR_PIN`
  - `LDR_CH`
- 如果你不再用 `ADC3`，还要一起改：
  - `ADCX`
  - `ADCX_CLK`
  - `DMA_CLK`
  - `DMA_STR`
  - `DMA_CH`
- 如果你只是关掉某一路，`AD_N` 和 `AD_POT_I / AD_TMP_I / AD_LDR_I` 会自动变化，但业务代码最好不要自己写数组下标，直接用 `AD_GetPotRaw()` 这些函数最稳
- 如果还要兼容旧的 `Pot_GetData()` 写法，必须保证 `AD_POT_EN = 1`

## 比赛直接抄的完整代码

### AD.h

```c
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
```

### AD.c

```c
#include "stm32f4xx.h"
#include "AD.h"
#include <math.h>

#define ADCX       ADC3
#define ADCX_CLK   RCC_APB2Periph_ADC3
#define DMA_CLK    RCC_AHB1Periph_DMA2
#define DMA_STR    DMA2_Stream0
#define DMA_CH     DMA_Channel_2
#define AD_SMP     ADC_SampleTime_144Cycles

#define POT_PORT   GPIOF
#define POT_CLK    RCC_AHB1Periph_GPIOF
#define POT_PIN    GPIO_Pin_7
#define POT_CH     ADC_Channel_5

#define TMP_PORT   GPIOA
#define TMP_CLK    RCC_AHB1Periph_GPIOA
#define TMP_PIN    GPIO_Pin_0
#define TMP_CH     ADC_Channel_0

#define LDR_PORT   GPIOA
#define LDR_CLK    RCC_AHB1Periph_GPIOA
#define LDR_PIN    GPIO_Pin_3
#define LDR_CH     ADC_Channel_3

volatile uint16_t AD_Buf[AD_N];

static uint8_t ad_ok = 0U;

/* 把三个模拟输入脚统一配置成模拟模式。 */
static void AD_GPIO_Init(void)
{
    GPIO_InitTypeDef gpio;

#if AD_POT_EN
    RCC_AHB1PeriphClockCmd(POT_CLK, ENABLE);
#endif

#if (AD_TMP_EN || AD_LDR_EN)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
#endif

    gpio.GPIO_Mode = GPIO_Mode_AN;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;

#if AD_POT_EN
    gpio.GPIO_Pin = POT_PIN;
    GPIO_Init(POT_PORT, &gpio);
#endif

#if AD_TMP_EN
    gpio.GPIO_Pin = TMP_PIN;
    GPIO_Init(TMP_PORT, &gpio);
#endif

#if AD_LDR_EN
    gpio.GPIO_Pin = LDR_PIN;
    GPIO_Init(LDR_PORT, &gpio);
#endif
}

/* DMA 把 ADC3 的结果循环搬到 AD_Buf[]。 */
static void AD_DMA_Init(void)
{
    DMA_InitTypeDef dma;

    RCC_AHB1PeriphClockCmd(DMA_CLK, ENABLE);

    DMA_Cmd(DMA_STR, DISABLE);
    while (DMA_GetCmdStatus(DMA_STR) != DISABLE)
    {
    }

    DMA_DeInit(DMA_STR);

    dma.DMA_Channel = DMA_CH;
    dma.DMA_PeripheralBaseAddr = (uint32_t)&ADCX->DR;
    dma.DMA_Memory0BaseAddr = (uint32_t)AD_Buf;
    dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
    dma.DMA_BufferSize = AD_N;
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
    DMA_Init(DMA_STR, &dma);
}

/* ADC3 连续扫描，按启用顺序采 Pot/Temp/LDR。 */
static void AD_ADC_Config(void)
{
    ADC_CommonInitTypeDef adc_com;
    ADC_InitTypeDef adc;
    uint8_t n = 1U;

    RCC_APB2PeriphClockCmd(ADCX_CLK, ENABLE);

    ADC_CommonStructInit(&adc_com);
    adc_com.ADC_Mode = ADC_Mode_Independent;
    adc_com.ADC_Prescaler = ADC_Prescaler_Div4;
    adc_com.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    adc_com.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&adc_com);

    ADC_StructInit(&adc);
    adc.ADC_Resolution = ADC_Resolution_12b;
    adc.ADC_ScanConvMode = ENABLE;
    adc.ADC_ContinuousConvMode = ENABLE;
    adc.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    adc.ADC_DataAlign = ADC_DataAlign_Right;
    adc.ADC_NbrOfConversion = AD_N;
    ADC_Init(ADCX, &adc);

#if AD_POT_EN
    ADC_RegularChannelConfig(ADCX, POT_CH, n++, AD_SMP);
#endif

#if AD_TMP_EN
    ADC_RegularChannelConfig(ADCX, TMP_CH, n++, AD_SMP);
#endif

#if AD_LDR_EN
    ADC_RegularChannelConfig(ADCX, LDR_CH, n++, AD_SMP);
#endif

    ADC_DMARequestAfterLastTransferCmd(ADCX, ENABLE);
    ADC_DMACmd(ADCX, ENABLE);
    ADC_Cmd(ADCX, ENABLE);
}

/* 只初始化一次，避免重复开 DMA 和 ADC。 */
void AD_Init(void)
{
    if (ad_ok != 0U)
    {
        return;
    }

    AD_GPIO_Init();
    AD_DMA_Init();
    AD_ADC_Config();

    DMA_Cmd(DMA_STR, ENABLE);
    ADC_SoftwareStartConv(ADCX);

    ad_ok = 1U;
}

/* 内部统一按索引读 DMA 缓冲。 */
static uint16_t AD_Read(uint8_t i)
{
    if (i >= AD_N)
    {
        return 0U;
    }

    return AD_Buf[i];
}

uint16_t AD_GetPotRaw(void)
{
#if AD_POT_EN
    return AD_Read(AD_POT_I);
#else
    return 0U;
#endif
}

uint16_t AD_GetTempRaw(void)
{
#if AD_TMP_EN
    return AD_Read(AD_TMP_I);
#else
    return 0U;
#endif
}

/* 热敏：返回摄氏温度整数值，比赛里串口直接 %d 打印。 */
int16_t AD_GetTempCelsius(void)
{
#if AD_TMP_EN
    float v = ((float)AD_GetTempRaw()) * (AD_VREF / AD_MAX);
    float r;
    float inv_t;
    float t;

    if (v < 0.01f)
    {
        v = 0.01f;
    }

    if (v > (AD_VREF - 0.01f))
    {
        v = AD_VREF - 0.01f;
    }

    r = TMP_R * v / (AD_VREF - v);
    if (r < 10.0f)
    {
        r = 10.0f;
    }

    inv_t = (1.0f / TMP_T0)
        + (1.0f / TMP_B) * (float)log((double)(r / TMP_R0));
    t = (1.0f / inv_t) - 273.15f;

    if (t >= 0.0f)
    {
        return (int16_t)(t + 0.5f);
    }

    return (int16_t)(t - 0.5f);
#else
    return 0;
#endif
}

uint16_t AD_GetLdrRaw(void)
{
#if AD_LDR_EN
    return AD_Read(AD_LDR_I);
#else
    return 0U;
#endif
}

/* 光敏：返回 Lux 整数值，比赛里够用。 */
uint16_t AD_GetLdrLux(void)
{
#if AD_LDR_EN
    float v = ((float)AD_GetLdrRaw()) * (AD_VREF / AD_MAX);
    float r;
    float lux;

    if (v < 0.01f)
    {
        v = 0.01f;
    }

    if (v > (AD_VREF - 0.01f))
    {
        v = AD_VREF - 0.01f;
    }

    r = v / (AD_VREF - v) * LDR_R;
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
#else
    return 0U;
#endif
}
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
