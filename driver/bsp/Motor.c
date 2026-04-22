#include "stm32f4xx.h"
#include "Motor.h"

/* 方向控制脚：PA4、PA6。 */
#define MOTOR_DIR_GPIO        GPIOA
#define MOTOR_IN1_PIN         GPIO_Pin_4
#define MOTOR_IN2_PIN         GPIO_Pin_6

/* PWM 输出脚：PA7 = TIM3_CH2。 */
#define MOTOR_PWM_GPIO        GPIOA
#define MOTOR_PWM_PIN         GPIO_Pin_7
#define MOTOR_PWM_PINSRC      GPIO_PinSource7
#define MOTOR_PWM_AF          GPIO_AF_TIM3
#define MOTOR_PWM_TIM         TIM3

static uint8_t motor_status = 0;
static int8_t motor_dir = 0;

/* 设置 PWM 占空比，范围 0~100。 */
static void motor_pwm_set(uint8_t duty)
{
    if (duty > 100U)
    {
        duty = 100U;
    }

    TIM_SetCompare2(MOTOR_PWM_TIM, duty);
}

/* 电机滑行停止：关 PWM，两路方向都拉低。 */
static void motor_stop(void)
{
    motor_pwm_set(0);
    GPIO_ResetBits(MOTOR_DIR_GPIO, MOTOR_IN1_PIN | MOTOR_IN2_PIN);
    motor_status = 0;
    motor_dir = 0;
}

/* 获取电机当前状态：0 停止，1 运行。 */
uint8_t Motor_get_status(void)
{
    return motor_status;
}

/* 初始化方向引脚和 PWM 输出。 */
void Motor_init(void)
{
    GPIO_InitTypeDef gpio_init;
    TIM_TimeBaseInitTypeDef tim_base;
    TIM_OCInitTypeDef tim_oc;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* PA4、PA6 做普通推挽输出，控制正反转。 */
    gpio_init.GPIO_Pin = MOTOR_IN1_PIN | MOTOR_IN2_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOTOR_DIR_GPIO, &gpio_init);

    /* PA7 复用成 TIM3_CH2，输出 PWM。 */
    GPIO_PinAFConfig(MOTOR_PWM_GPIO, MOTOR_PWM_PINSRC, MOTOR_PWM_AF);
    gpio_init.GPIO_Pin = MOTOR_PWM_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_AF;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOTOR_PWM_GPIO, &gpio_init);

    /* 168MHz 主频下，TIM3 计数频率设为 2MHz，ARR=99，PWM 频率约 20kHz。 */
    TIM_InternalClockConfig(MOTOR_PWM_TIM);
    tim_base.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_base.TIM_CounterMode = TIM_CounterMode_Up;
    tim_base.TIM_Period = 100 - 1;
    tim_base.TIM_Prescaler = 42 - 1;
    tim_base.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(MOTOR_PWM_TIM, &tim_base);

    TIM_OCStructInit(&tim_oc);
    tim_oc.TIM_OCMode = TIM_OCMode_PWM1;
    tim_oc.TIM_OutputState = TIM_OutputState_Enable;
    tim_oc.TIM_OCPolarity = TIM_OCPolarity_High;
    tim_oc.TIM_Pulse = 0;
    TIM_OC2Init(MOTOR_PWM_TIM, &tim_oc);
    TIM_OC2PreloadConfig(MOTOR_PWM_TIM, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(MOTOR_PWM_TIM, ENABLE);
    TIM_Cmd(MOTOR_PWM_TIM, ENABLE);

    motor_stop();
}

/* 设置电机速度：正数正转，负数反转，0 停止。 */
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
        motor_stop();
        return;
    }

    dir = (speed > 0) ? 1 : -1;
    duty = (speed > 0) ? (uint8_t)speed : (uint8_t)(-speed);

    /* 换向前先关 PWM，减少直通电流。 */
    if ((motor_dir != 0) && (motor_dir != dir))
    {
        motor_stop();
    }

    if (dir > 0)
    {
        GPIO_SetBits(MOTOR_DIR_GPIO, MOTOR_IN1_PIN);
        GPIO_ResetBits(MOTOR_DIR_GPIO, MOTOR_IN2_PIN);
    }
    else
    {
        GPIO_ResetBits(MOTOR_DIR_GPIO, MOTOR_IN1_PIN);
        GPIO_SetBits(MOTOR_DIR_GPIO, MOTOR_IN2_PIN);
    }

    motor_pwm_set(duty);
    motor_status = 1;
    motor_dir = dir;
}
