#include "stm32f4xx.h"
#include "Servo.h"

/* 舵机输出引脚：PA1 = TIM2_CH2 */
#define SERVO_GPIO_PORT      GPIOA
#define SERVO_GPIO_PIN       GPIO_Pin_1
#define SERVO_GPIO_PINSRC    GPIO_PinSource1
#define SERVO_GPIO_AF        GPIO_AF_TIM2

#define SERVO_TIM            TIM2
#define SERVO_PERIOD_US      20000U
#define SERVO_MIN_PULSE_US   500U
#define SERVO_MAX_PULSE_US   2500U

/* 设置舵机 PWM 脉宽，单位是 us */
static void servo_set_compare(uint16_t compare)
{
    if (compare < SERVO_MIN_PULSE_US)
    {
        compare = SERVO_MIN_PULSE_US;
    }
    else if (compare > SERVO_MAX_PULSE_US)
    {
        compare = SERVO_MAX_PULSE_US;
    }

    TIM_SetCompare2(SERVO_TIM, compare);
}

/* 初始化舵机 PWM，使用 TIM2_CH2(PA1) 输出 50Hz 控制波形 */
void Servo_Init(void)
{
    GPIO_InitTypeDef gpio_init;
    TIM_TimeBaseInitTypeDef tim_base;
    TIM_OCInitTypeDef tim_oc;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    GPIO_PinAFConfig(SERVO_GPIO_PORT, SERVO_GPIO_PINSRC, SERVO_GPIO_AF);
    gpio_init.GPIO_Pin = SERVO_GPIO_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_AF;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SERVO_GPIO_PORT, &gpio_init);

    TIM_InternalClockConfig(SERVO_TIM);
    tim_base.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_base.TIM_CounterMode = TIM_CounterMode_Up;
    tim_base.TIM_Period = SERVO_PERIOD_US - 1U;
    tim_base.TIM_Prescaler = 84 - 1U;
    tim_base.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(SERVO_TIM, &tim_base);

    TIM_OCStructInit(&tim_oc);
    tim_oc.TIM_OCMode = TIM_OCMode_PWM1;
    tim_oc.TIM_OutputState = TIM_OutputState_Enable;
    tim_oc.TIM_OCPolarity = TIM_OCPolarity_High;
    tim_oc.TIM_Pulse = 1500U;
    TIM_OC2Init(SERVO_TIM, &tim_oc);
    TIM_OC2PreloadConfig(SERVO_TIM, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(SERVO_TIM, ENABLE);
    TIM_Cmd(SERVO_TIM, ENABLE);
}

/* 设置舵机角度，范围 0~180 度 */
void Servo_SetAngle(float Angle)
{
    uint16_t compare;

    if (Angle < 0.0f)
    {
        Angle = 0.0f;
    }
    else if (Angle > 180.0f)
    {
        Angle = 180.0f;
    }

    compare = (uint16_t)(SERVO_MIN_PULSE_US + (Angle * 2000.0f / 180.0f));
    servo_set_compare(compare);
}
