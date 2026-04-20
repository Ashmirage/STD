#include "mq2.h"
#include "my_time.h"
#include "math.h"
#include "AD.h"

// ====== 新增：按你的硬件实际情况改这些 ======
// STM32F103 ADC 通常参考电压是 3.3V（VDDA）
#define MQ2_ADC_VREF        (3.3f)
#define MQ2_ADC_MAX         (4095.0f)

// MQ2模块的传感器电路供电（很多模块是 5V）
#define MQ2_VC              (5.0f)

// 如果 MQ2 AO 经过分压后进 ADC：Vout_real = Vadc * MQ2_AO_SCALE
// 没分压就填 1.0f；比如用 20k:10k 分压（上20k下10k），比例= (20+10)/10 = 3.0
#define MQ2_AO_SCALE        (1.0f)

// 你的负载电阻 RL（单位：kΩ），按模块实际RL填写（常见 1k/5k/10k）
#define MQ2_RL_KOHM         (1.0f)

// MQ2初始化函数
void MQ2_Init(void)
{
	AD_init();
}

uint16_t mq2_data;


// 获取采样值
uint16_t MQ2_GetData(void)
{

	#if MODE
	uint32_t  tempData = 0;
	for (uint8_t i = 0; i < MQ2_READ_TIMES; i++)
	{
		tempData += AD_Value[0];
		Delay_ms(5);
	}
	tempData /= MQ2_READ_TIMES;
	return tempData;

	#else
	uint16_t tempData;
	tempData = !HAL_GPIO_ReadPin(MQ2_DO_GPIO_PORT, MQ2_DO_GPIO_PIN);
	return tempData;
	#endif
}


// 获取浓度数据
float MQ2_GetData_PPM(void)
{
#if MODE
    float adc = 0;
    for (uint8_t i = 0; i < MQ2_READ_TIMES; i++)
    {
        adc += (float)AD_Value[0];
        Delay_ms(5);
    }
    adc /= MQ2_READ_TIMES;

    // ADC 采到的电压（0~Vref）
    float v_adc = (adc * MQ2_ADC_VREF) / MQ2_ADC_MAX;

    // 还原 MQ2 AO 真实电压（如果做了分压，这里乘回去）
    float v_out = v_adc * MQ2_AO_SCALE;

    // 防止除0/防止接近VC导致Rs趋近0
    if (v_out < 0.01f) v_out = 0.01f;
    if (v_out > (MQ2_VC - 0.01f)) v_out = (MQ2_VC - 0.01f);

    // 标准分压模型：Vout = Vc * RL / (RL + Rs)  => Rs = RL * (Vc/Vout - 1)
    float RS = MQ2_RL_KOHM * (MQ2_VC / v_out - 1.0f);

    // 这里 R0/曲线系数你需要按“洁净空气标定 + datasheet曲线”来定
    float R0 = 6.64f;

    // 你的原公式先保留，但注意：它只有在你系数/气体曲线匹配时才有意义
    float ppm = powf(11.5428f * R0 / RS, 0.6549f);

    return ppm;
#else
    return 0.0f;
#endif
}
