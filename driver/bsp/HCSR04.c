#include "stm32f4xx.h"
#include "HCSR04.h"
#include "delay.h"

/* 默认接线：PE1=TRIG，PE3=ECHO，TIM5 计时 */
#define T_IO      GPIOE
#define T_CLK     RCC_AHB1Periph_GPIOE
#define T_PIN     GPIO_Pin_1

#define E_IO      GPIOE
#define E_CLK     RCC_AHB1Periph_GPIOE
#define E_PIN     GPIO_Pin_3

#define TIMX      TIM5
#define TIM_CLK   RCC_APB1Periph_TIM5
#define WAIT_US   30000U

/* TRIG 输出和 ECHO 读取 */
#define T0()      GPIO_ResetBits(T_IO, T_PIN)
#define T1()      GPIO_SetBits(T_IO, T_PIN)
#define E()       (GPIO_ReadInputDataBit(E_IO, E_PIN) == Bit_SET)

static uint8_t ok = 0;

/* 获取 TIM5 的真实计数时钟，用来分频成 1MHz */
static uint32_t tim_clk(void)
{
    RCC_ClocksTypeDef rcc;

    RCC_GetClocksFreq(&rcc);

    if ((RCC->CFGR & RCC_CFGR_PPRE1) == RCC_CFGR_PPRE1_DIV1)
    {
        return rcc.PCLK1_Frequency;
    }

    return rcc.PCLK1_Frequency * 2U;
}

/* 初始化 TRIG/ECHO 引脚和 1us 计数定时器 */
static void init_hw(void)
{
    GPIO_InitTypeDef gpio;
    TIM_TimeBaseInitTypeDef tim;

    RCC_AHB1PeriphClockCmd(T_CLK | E_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(TIM_CLK, ENABLE);

    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = T_PIN;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(T_IO, &gpio);

    gpio.GPIO_Pin = E_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(E_IO, &gpio);

    T0();

    TIM_TimeBaseStructInit(&tim);
    tim.TIM_Prescaler = (uint16_t)(tim_clk() / 1000000U - 1U);
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_Period = 0xFFFFFFFFU;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIMX, &tim);
    TIM_SetCounter(TIMX, 0);
    TIM_Cmd(TIMX, ENABLE);

    ok = 1;
}

/* 等待 ECHO 变成指定电平，超过 WAIT_US 就失败 */
static uint8_t wait_e(uint8_t v)
{
    uint32_t start = TIM_GetCounter(TIMX);

    while ((uint8_t)E() != v)
    {
        if ((uint32_t)(TIM_GetCounter(TIMX) - start) >= WAIT_US)
        {
            return 0;
        }
    }

    return 1;
}

/* 初始化超声波模块 */
void HCSR04_Init(void)
{
    init_hw();
}

/* 读取距离：成功返回 1，失败返回 0，单位 cm */
uint8_t HCSR04_Read(float *cm)
{
    uint32_t start;
    if (cm == 0)
    {
        return 0;
    }

    if (!ok)
    {
        HCSR04_Init();
    }

    if (!wait_e(0))
    {
        return 0;
    }

    T0();
    delay_us(2);
    T1();
    delay_us(10);
    T0();

    if (!wait_e(1))
    {
        return 0;
    }

    start = TIM_GetCounter(TIMX);

    if (!wait_e(0))
    {
        return 0;
    }

    *cm = (float)(TIM_GetCounter(TIMX) - start) * 0.01715f;

    return 1;
}
