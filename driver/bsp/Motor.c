#include "stm32f4xx.h"
#include "Motor.h"

/* IN1=PA4, IN2=PA6, PWM=PA7(TIM3_CH2) */
#define IO        GPIOA
#define IN1       GPIO_Pin_4
#define IN2       GPIO_Pin_6
#define PWM       GPIO_Pin_7
#define PWM_SRC   GPIO_PinSource7
#define PWM_AF    GPIO_AF_TIM3
#define TIMX      TIM3

static uint8_t run = 0;
static int8_t dir_now = 0;

static void pwm_set(uint8_t duty)
{
    if (duty > 100U)
    {
        duty = 100U;
    }

    TIM_SetCompare2(TIMX, duty);
}

static void stop(void)
{
    pwm_set(0);
    GPIO_ResetBits(IO, IN1 | IN2);
    run = 0;
    dir_now = 0;
}

uint8_t Motor_get_status(void)
{
    return run;
}

void Motor_init(void)
{
    GPIO_InitTypeDef gpio;
    TIM_TimeBaseInitTypeDef tim;
    TIM_OCInitTypeDef oc;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    gpio.GPIO_Pin = IN1 | IN2;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IO, &gpio);

    GPIO_PinAFConfig(IO, PWM_SRC, PWM_AF);
    gpio.GPIO_Pin = PWM;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IO, &gpio);

    /* 20 kHz PWM when TIM3 clock is 84 MHz. */
    TIM_InternalClockConfig(TIMX);
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_Period = 100 - 1;
    tim.TIM_Prescaler = 42 - 1;
    tim.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIMX, &tim);

    TIM_OCStructInit(&oc);
    oc.TIM_OCMode = TIM_OCMode_PWM1;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_OCPolarity = TIM_OCPolarity_High;
    oc.TIM_Pulse = 0;
    TIM_OC2Init(TIMX, &oc);
    TIM_OC2PreloadConfig(TIMX, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIMX, ENABLE);
    TIM_Cmd(TIMX, ENABLE);

    stop();
}

void Motor_set_speed(int8_t speed)
{
    uint8_t duty;
    int8_t dir;

    if (speed > 100)
    {
        speed = 100;
    }
    else if (speed < -100)
    {
        speed = -100;
    }

    if (speed == 0)
    {
        stop();
        return;
    }

    dir = (speed > 0) ? 1 : -1;
    duty = (speed > 0) ? (uint8_t)speed : (uint8_t)(-speed);

    if ((dir_now != 0) && (dir_now != dir))
    {
        stop();
    }

    if (dir > 0)
    {
        GPIO_SetBits(IO, IN1);
        GPIO_ResetBits(IO, IN2);
    }
    else
    {
        GPIO_ResetBits(IO, IN1);
        GPIO_SetBits(IO, IN2);
    }

    pwm_set(duty);
    run = 1;
    dir_now = dir;
}
