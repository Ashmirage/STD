#include "HCSR04.h"
#include "delay.h"

#define TIMX HCSR04_TIM
#define T0() GPIO_ResetBits(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN)
#define T1() GPIO_SetBits(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN)
#define E()  (GPIO_ReadInputDataBit(HCSR04_ECHO_PORT, HCSR04_ECHO_PIN) == Bit_SET)

static uint8_t x = 0;

/* 计算 TIMx 当前实际计数时钟，后面统一分频到 1MHz。 */
static uint32_t c(void)
{
    RCC_ClocksTypeDef r;
    RCC_GetClocksFreq(&r);
    return ((RCC->CFGR & RCC_CFGR_PPRE1) == RCC_CFGR_PPRE1_DIV1) ? r.PCLK1_Frequency : (r.PCLK1_Frequency * 2);
}

static void i(void)
{
    GPIO_InitTypeDef g;
    TIM_TimeBaseInitTypeDef t;

    RCC_AHB1PeriphClockCmd(HCSR04_TRIG_CLK | HCSR04_ECHO_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(HCSR04_TIM_CLK, ENABLE);

    GPIO_StructInit(&g);
    g.GPIO_Pin = HCSR04_TRIG_PIN;
    g.GPIO_Mode = GPIO_Mode_OUT;
    g.GPIO_OType = GPIO_OType_PP;
    g.GPIO_PuPd = GPIO_PuPd_NOPULL;
    g.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HCSR04_TRIG_PORT, &g);

    g.GPIO_Pin = HCSR04_ECHO_PIN;
    g.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(HCSR04_ECHO_PORT, &g);

    T0();
    TIM_TimeBaseStructInit(&t);
    t.TIM_Prescaler = (uint16_t)(c() / 1000000 - 1);
    t.TIM_CounterMode = TIM_CounterMode_Up;
    t.TIM_Period = 0xFFFFFFFF;
    t.TIM_ClockDivision = TIM_CKD_DIV1;
    t.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIMX, &t);
    TIM_SetCounter(TIMX, 0);
    TIM_Cmd(TIMX, ENABLE);
    x = 1;
}

/* 等待 ECHO 变成目标电平，超时就直接返回失败。 */
static uint8_t w(uint8_t v)
{
    uint32_t s = TIM_GetCounter(TIMX);
    while ((uint8_t)E() != v)
    {
        if ((uint32_t)(TIM_GetCounter(TIMX) - s) >= HCSR04_TIMEOUT_US)
        {
            return 0;
        }
    }
    return 1;
}

/* 初始化超声波模块，默认用 PE1 做 TRIG，PE3 做 ECHO。 */
void HCSR04_Init(void)
{
    i();
}

/* 读一次距离，成功返回 1，距离单位 cm。 */
uint8_t HCSR04_Read(float *cm)
{
    uint32_t s;
    if (cm == 0)
    {
        return 0;
    }
    if (!x)
    {
        HCSR04_Init();
    }
    if (!w(0))
    {
        return 0;
    }
    T0();
    delay_us(2);
    T1();
    delay_us(10);
    T0();
    if (!w(1))
    {
        return 0;
    }
    s = TIM_GetCounter(TIMX);
    if (!w(0))
    {
        return 0;
    }
    *cm = (float)(TIM_GetCounter(TIMX) - s) * 0.01715f;
    return 1;
}
