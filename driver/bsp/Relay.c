#include "stm32f4xx.h"
#include "Relay.h"

/* 默认使用 PC4 控制继电器输入端。 */
#define RELAY_GPIO_PORT      GPIOC
#define RELAY_GPIO_PIN       GPIO_Pin_4
#define RELAY_GPIO_CLK       RCC_AHB1Periph_GPIOC

/* 初始化继电器控制脚，默认拉低，避免上电误吸合。 */
void Relay_init(void)
{
    GPIO_InitTypeDef gpio_init;

    RCC_AHB1PeriphClockCmd(RELAY_GPIO_CLK, ENABLE);

    gpio_init.GPIO_Pin = RELAY_GPIO_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RELAY_GPIO_PORT, &gpio_init);

    Relay_off();
}

/* 继电器吸合：NO 与 COM 接通。 */
void Relay_on(void)
{
    GPIO_SetBits(RELAY_GPIO_PORT, RELAY_GPIO_PIN);
}

/* 继电器释放：NO 与 COM 断开。 */
void Relay_off(void)
{
    GPIO_ResetBits(RELAY_GPIO_PORT, RELAY_GPIO_PIN);
}

/* 根据传入状态切换继电器。 */
void Relay_status(uint8_t status)
{
    if (status != 0U)
    {
        Relay_on();
    }
    else
    {
        Relay_off();
    }
}
