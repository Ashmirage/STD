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
