#include "stm32f4xx.h"
#include "Pot.h"

/* 当前把电位器接在 PA5，对应板上的 STM_ADC / ADC1_IN5。
   如果使用板载 RV1，需要在多功能端口把 RV1 滑动端接到 STM_ADC。 */
#define POT_GPIO_PORT        GPIOA
#define POT_GPIO_PIN         GPIO_Pin_5
#define POT_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define POT_ADC              ADC1
#define POT_ADC_CLK          RCC_APB2Periph_ADC1
#define POT_ADC_CHANNEL      ADC_Channel_5
#define POT_ADC_SAMPLE_TIME  ADC_SampleTime_144Cycles

static uint8_t pot_inited = 0;

/* 初始化电位器 ADC，使用单通道轮询读取，方便直接调用。 */
void Pot_Init(void)
{
    GPIO_InitTypeDef gpio_init;
    ADC_CommonInitTypeDef adc_common_init;
    ADC_InitTypeDef adc_init;

    RCC_AHB1PeriphClockCmd(POT_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(POT_ADC_CLK, ENABLE);

    gpio_init.GPIO_Pin = POT_GPIO_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_AN;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(POT_GPIO_PORT, &gpio_init);

    ADC_CommonStructInit(&adc_common_init);
    adc_common_init.ADC_Mode = ADC_Mode_Independent;
    adc_common_init.ADC_Prescaler = ADC_Prescaler_Div4;
    adc_common_init.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    adc_common_init.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&adc_common_init);

    ADC_StructInit(&adc_init);
    adc_init.ADC_Resolution = ADC_Resolution_12b;
    adc_init.ADC_ScanConvMode = DISABLE;
    adc_init.ADC_ContinuousConvMode = DISABLE;
    adc_init.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    adc_init.ADC_DataAlign = ADC_DataAlign_Right;
    adc_init.ADC_NbrOfConversion = 1;
    ADC_Init(POT_ADC, &adc_init);

    ADC_RegularChannelConfig(POT_ADC, POT_ADC_CHANNEL, 1, POT_ADC_SAMPLE_TIME);
    ADC_Cmd(POT_ADC, ENABLE);

    pot_inited = 1;
}

/* 读取一次电位器 ADC 值。 */
uint16_t Pot_GetData(void)
{
    if (pot_inited == 0U)
    {
        Pot_Init();
    }

    ADC_ClearFlag(POT_ADC, ADC_FLAG_EOC);
    ADC_SoftwareStartConv(POT_ADC);

    while (ADC_GetFlagStatus(POT_ADC, ADC_FLAG_EOC) == RESET)
    {
    }

    return ADC_GetConversionValue(POT_ADC);
}

/* 把 ADC 原始值换算成电压值。 */
float Pot_GetVoltage(void)
{
    return Pot_GetData() * (3.3f / 4095.0f);
}
