#include "remote.h"

/* 红外接收头接 PA8，对应 TIM1_CH1。 */
#define REMOTE_GPIO_PORT       GPIOA
#define REMOTE_GPIO_PIN        GPIO_Pin_8
#define REMOTE_GPIO_PINSRC     GPIO_PinSource8
#define REMOTE_GPIO_AF         GPIO_AF_TIM1

#define REMOTE_TIM             TIM1
#define REMOTE_TIM_CHANNEL     TIM_Channel_1

#define RDATA                  GPIO_ReadInputDataBit(REMOTE_GPIO_PORT, REMOTE_GPIO_PIN)

#define R_OK                   0x80U
#define R_END                  0x40U
#define R_EDGE                 0x10U
#define R_OVF                  0x0FU

#define R0_MIN                 300U
#define R0_MAX                 800U
#define R1_MIN                 1400U
#define R1_MAX                 1800U
#define RR_MIN                 2000U
#define RR_MAX                 3000U
#define RH_MIN                 4200U
#define RH_MAX                 4700U

static uint8_t r_sta = 0;
static uint32_t r_dat = 0;
uint8_t g_remote_cnt = 0;

/* 切换捕获边沿，用来量高电平宽度。 */
static void remote_set_edge(uint16_t edge)
{
    REMOTE_TIM->CCER &= (uint16_t)~TIM_CCER_CC1E;
    REMOTE_TIM->CCER &= (uint16_t)~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
    REMOTE_TIM->CCER |= (uint16_t)(edge | TIM_CCER_CC1E);
}

/* 红外接收头初始化，当前用 PA8/TIM1_CH1。 */
void remote_init(void)
{
    GPIO_InitTypeDef gpio;
    TIM_TimeBaseInitTypeDef tim;
    TIM_ICInitTypeDef ic;
    NVIC_InitTypeDef nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    GPIO_PinAFConfig(REMOTE_GPIO_PORT, REMOTE_GPIO_PINSRC, REMOTE_GPIO_AF);
    gpio.GPIO_Pin = REMOTE_GPIO_PIN;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(REMOTE_GPIO_PORT, &gpio);

    TIM_DeInit(REMOTE_TIM);
    tim.TIM_Prescaler = 168 - 1;
    tim.TIM_Period = 10000 - 1;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(REMOTE_TIM, &tim);

    ic.TIM_Channel = REMOTE_TIM_CHANNEL;
    ic.TIM_ICPolarity = TIM_ICPolarity_Rising;
    ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
    ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    ic.TIM_ICFilter = 0x03;
    TIM_ICInit(REMOTE_TIM, &ic);

    TIM_ClearITPendingBit(REMOTE_TIM, TIM_IT_Update | TIM_IT_CC1);
    TIM_ITConfig(REMOTE_TIM, TIM_IT_Update | TIM_IT_CC1, ENABLE);

    nvic.NVIC_IRQChannelPreemptionPriority = 2;
    nvic.NVIC_IRQChannelCmd = ENABLE;

    nvic.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
    nvic.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&nvic);

    nvic.NVIC_IRQChannel = TIM1_CC_IRQn;
    nvic.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&nvic);

    r_sta = 0;
    r_dat = 0;
    g_remote_cnt = 0;
    TIM_Cmd(REMOTE_TIM, ENABLE);
}

/* TIM1 更新中断。 */
void TIM1_UP_TIM10_IRQHandler(void)
{
    if (TIM_GetITStatus(REMOTE_TIM, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(REMOTE_TIM, TIM_IT_Update);
        Remote_TIM_ElapsedCallback();
    }
}

/* TIM1 捕获中断。 */
void TIM1_CC_IRQHandler(void)
{
    if (TIM_GetITStatus(REMOTE_TIM, TIM_IT_CC1) != RESET)
    {
        TIM_ClearITPendingBit(REMOTE_TIM, TIM_IT_CC1);
        Remote_IC_CaptureCallback();
    }
}

/* 更新中断里调用，用来判断一帧是不是收完了。 */
void Remote_TIM_ElapsedCallback(void)
{
    if ((r_sta & R_OK) == 0U)
    {
        return;
    }

    r_sta &= (uint8_t)~R_EDGE;

    if ((r_sta & R_OVF) == 0U)
    {
        r_sta |= R_END;
    }

    if ((r_sta & R_OVF) < 14U)
    {
        r_sta++;
        return;
    }

    r_sta &= (uint8_t)~R_OK;
    r_sta &= 0xF0U;
}

/* 捕获中断里调用，用来解析 NEC 脉宽。 */
void Remote_IC_CaptureCallback(void)
{
    uint16_t cap;

    if (RDATA)
    {
        remote_set_edge(TIM_ICPolarity_Falling);
        TIM_SetCounter(REMOTE_TIM, 0);
        r_sta |= R_EDGE;
        return;
    }

    cap = (uint16_t)TIM_GetCapture1(REMOTE_TIM);
    remote_set_edge(TIM_ICPolarity_Rising);

    if ((r_sta & R_EDGE) == 0U)
    {
        return;
    }

    if (r_sta & R_OK)
    {
        if ((cap > R0_MIN) && (cap < R0_MAX))
        {
            r_dat >>= 1;
        }
        else if ((cap > R1_MIN) && (cap < R1_MAX))
        {
            r_dat = (r_dat >> 1) | 0x80000000U;
        }
        else if ((cap > RR_MIN) && (cap < RR_MAX))
        {
            g_remote_cnt++;
            r_sta &= 0xF0U;
        }
    }
    else if ((cap > RH_MIN) && (cap < RH_MAX))
    {
        r_sta |= R_OK;
        r_dat = 0;
        g_remote_cnt = 0;
    }

    r_sta &= (uint8_t)~R_EDGE;
}

/* 扫描一次按键，没按到返回 0。 */
uint8_t remote_scan(void)
{
    uint8_t a;
    uint8_t na;
    uint8_t k;
    uint8_t nk;

    if ((r_sta & R_END) == 0U)
    {
        return 0U;
    }

    a = (uint8_t)r_dat;
    na = (uint8_t)(r_dat >> 8);
    k = (uint8_t)(r_dat >> 16);
    nk = (uint8_t)(r_dat >> 24);

    if ((a == (uint8_t)(~na)) &&
        (a == REMOTE_ID) &&
        (k == (uint8_t)(~nk)) &&
        ((r_sta & R_OK) != 0U))
    {
        return k;
    }

    r_sta &= (uint8_t)~R_END;
    g_remote_cnt = 0;
    return 0U;
}
