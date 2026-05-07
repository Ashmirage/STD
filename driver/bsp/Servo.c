#include "stm32f4xx.h"
#include "Servo.h"

/* 舵机输出脚：PA1 = TIM2_CH2 */
#define IO     GPIOA
#define PIN    GPIO_Pin_1
#define SRC    GPIO_PinSource1
#define AF     GPIO_AF_TIM2
#define TIMX   TIM2

/* 20ms 周期，0.5ms~2.5ms 脉宽对应 0~180 度 */
#define PER    20000U
#define MIN    500U
#define MAX    2500U

/* 设置 PWM 脉宽，单位：us */
static void pwm_set(uint16_t us)
{
    if (us < MIN)
    {
        us = MIN;
    }
    else if (us > MAX)
    {
        us = MAX;
    }

    TIM_SetCompare2(TIMX, us);
}

/* 初始化 PA1 的 TIM2_CH2 PWM，频率 50Hz */
void Servo_Init(void)
{
    GPIO_InitTypeDef gpio;
    TIM_TimeBaseInitTypeDef tim;
    TIM_OCInitTypeDef oc;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    GPIO_PinAFConfig(IO, SRC, AF);
    gpio.GPIO_Pin = PIN;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IO, &gpio);

    /* TIM2 计数频率设为 1MHz，1 个计数 = 1us */
    TIM_InternalClockConfig(TIMX);
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_Period = PER - 1U;
    tim.TIM_Prescaler = 84 - 1U;
    tim.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIMX, &tim);

    TIM_OCStructInit(&oc);
    oc.TIM_OCMode = TIM_OCMode_PWM1;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_OCPolarity = TIM_OCPolarity_High;
    oc.TIM_Pulse = 1500U;
    TIM_OC2Init(TIMX, &oc);
    TIM_OC2PreloadConfig(TIMX, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIMX, ENABLE);
    TIM_Cmd(TIMX, ENABLE);
}

/* 设置舵机角度，范围：0~180 度 */
void Servo_SetAngle(float angle)
{
    uint16_t us;

    if (angle < 0.0f)
    {
        angle = 0.0f;
    }
    else if (angle > 180.0f)
    {
        angle = 180.0f;
    }

    us = (uint16_t)(MIN + (angle * 2000.0f / 180.0f));
    pwm_set(us);
}
