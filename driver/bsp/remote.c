#include "stm32f4xx.h"
#include "remote.h"

/* 红外接收头接在 PA8，对应 TIM1_CH1 */
#define REMOTE_GPIO_PORT       GPIOA
#define REMOTE_GPIO_PIN        GPIO_Pin_8
#define REMOTE_GPIO_PINSRC     GPIO_PinSource8
#define REMOTE_GPIO_AF         GPIO_AF_TIM1

#define REMOTE_TIM             TIM1
#define REMOTE_TIM_CHANNEL     TIM_Channel_1

/* 读取红外接收头当前电平 */
#define RDATA                  GPIO_ReadInputDataBit(REMOTE_GPIO_PORT, REMOTE_GPIO_PIN)

/* 接收状态位
 * bit7: 已收到引导码
 * bit6: 一帧按键数据接收完成
 * bit4: 上升沿已经捕获
 * bit3~0: 溢出计数
 */
static uint8_t g_remote_sta = 0;
static uint32_t g_remote_data = 0;
uint8_t g_remote_cnt = 0;

/* 切换 TIM1_CH1 的捕获边沿，便于测量高电平宽度 */
static void remote_set_capture_polarity(uint16_t polarity)
{
    REMOTE_TIM->CCER &= (uint16_t)~TIM_CCER_CC1E;
    REMOTE_TIM->CCER &= (uint16_t)~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
    REMOTE_TIM->CCER |= (uint16_t)(polarity | TIM_CCER_CC1E);
}

/* 初始化红外接收头输入捕获，使用 PA8/TIM1_CH1 */
void remote_init(void)
{
    GPIO_InitTypeDef gpio_init;
    TIM_TimeBaseInitTypeDef tim_base;
    TIM_ICInitTypeDef tim_ic;
    NVIC_InitTypeDef nvic_init;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    GPIO_PinAFConfig(REMOTE_GPIO_PORT, REMOTE_GPIO_PINSRC, REMOTE_GPIO_AF);
    gpio_init.GPIO_Pin = REMOTE_GPIO_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_AF;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(REMOTE_GPIO_PORT, &gpio_init);

    TIM_DeInit(REMOTE_TIM);

    TIM_TimeBaseStructInit(&tim_base);
    tim_base.TIM_Prescaler = 168 - 1;
    tim_base.TIM_Period = 10000 - 1;
    tim_base.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_base.TIM_CounterMode = TIM_CounterMode_Up;
    tim_base.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(REMOTE_TIM, &tim_base);

    TIM_ICStructInit(&tim_ic);
    tim_ic.TIM_Channel = REMOTE_TIM_CHANNEL;
    tim_ic.TIM_ICPolarity = TIM_ICPolarity_Rising;
    tim_ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
    tim_ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    tim_ic.TIM_ICFilter = 0x03;
    TIM_ICInit(REMOTE_TIM, &tim_ic);

    TIM_ClearFlag(REMOTE_TIM, TIM_FLAG_Update | TIM_FLAG_CC1);
    TIM_ClearITPendingBit(REMOTE_TIM, TIM_IT_Update | TIM_IT_CC1);
    TIM_ITConfig(REMOTE_TIM, TIM_IT_Update | TIM_IT_CC1, ENABLE);

    nvic_init.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 2;
    nvic_init.NVIC_IRQChannelSubPriority = 1;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);

    nvic_init.NVIC_IRQChannel = TIM1_CC_IRQn;
    nvic_init.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&nvic_init);

    g_remote_sta = 0;
    g_remote_data = 0;
    g_remote_cnt = 0;

    TIM_Cmd(REMOTE_TIM, ENABLE);
}

/* TIM1 更新中断，判断当前红外一帧数据是否接收结束 */
void TIM1_UP_TIM10_IRQHandler(void)
{
    if (TIM_GetITStatus(REMOTE_TIM, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(REMOTE_TIM, TIM_IT_Update);
        Remote_TIM_ElapsedCallback();
    }
}

/* TIM1 捕获中断，读取脉宽并交给红外解码逻辑 */
void TIM1_CC_IRQHandler(void)
{
    if (TIM_GetITStatus(REMOTE_TIM, TIM_IT_CC1) != RESET)
    {
        TIM_ClearITPendingBit(REMOTE_TIM, TIM_IT_CC1);
        Remote_IC_CaptureCallback();
    }
}

/* 定时器更新中断里调用，用来判断一帧数据是否接收完成 */
void Remote_TIM_ElapsedCallback(void)
{
    if (g_remote_sta & 0x80)
    {
        g_remote_sta &= (uint8_t)~0x10;

        if ((g_remote_sta & 0x0F) == 0x00)
        {
            g_remote_sta |= 1U << 6;
        }

        if ((g_remote_sta & 0x0F) < 14U)
        {
            g_remote_sta++;
        }
        else
        {
            g_remote_sta &= (uint8_t)~(1U << 7);
            g_remote_sta &= 0xF0;
        }
    }
}

/* 输入捕获中断里调用，用来解析 NEC 红外脉宽数据 */
void Remote_IC_CaptureCallback(void)
{
    uint16_t dval;

    if (RDATA)
    {
        remote_set_capture_polarity(TIM_ICPolarity_Falling);
        TIM_SetCounter(REMOTE_TIM, 0);
        g_remote_sta |= 0x10;
    }
    else
    {
        dval = (uint16_t)TIM_GetCapture1(REMOTE_TIM);
        remote_set_capture_polarity(TIM_ICPolarity_Rising);

        if (g_remote_sta & 0x10)
        {
            if (g_remote_sta & 0x80)
            {
                if ((dval > 300U) && (dval < 800U))
                {
                    g_remote_data >>= 1;
                    g_remote_data &= 0x7FFFFFFFU;
                }
                else if ((dval > 1400U) && (dval < 1800U))
                {
                    g_remote_data >>= 1;
                    g_remote_data |= 0x80000000U;
                }
                else if ((dval > 2000U) && (dval < 3000U))
                {
                    g_remote_cnt++;
                    g_remote_sta &= 0xF0;
                }
            }
            else if ((dval > 4200U) && (dval < 4700U))
            {
                g_remote_sta |= 1U << 7;
                g_remote_cnt = 0;
            }
        }

        g_remote_sta &= (uint8_t)~(1U << 4);
    }
}

/* 扫描一次红外按键，返回按键码，没按键时返回 0 */
uint8_t remote_scan(void)
{
    uint8_t sta = 0;
    uint8_t t1;
    uint8_t t2;

    if (g_remote_sta & (1U << 6))
    {
        t1 = (uint8_t)g_remote_data;
        t2 = (uint8_t)((g_remote_data >> 8) & 0xFFU);

        if ((t1 == (uint8_t)(~t2)) && (t1 == REMOTE_ID))
        {
            t1 = (uint8_t)((g_remote_data >> 16) & 0xFFU);
            t2 = (uint8_t)((g_remote_data >> 24) & 0xFFU);

            if (t1 == (uint8_t)(~t2))
            {
                sta = t1;
            }
        }

        if ((sta == 0U) || ((g_remote_sta & 0x80) == 0U))
        {
            g_remote_sta &= (uint8_t)~(1U << 6);
            g_remote_cnt = 0;
        }
    }

    return sta;
}
