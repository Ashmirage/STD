#include "DHT11.h"
#include "delay.h"

#define DL 20
#define DR 30
#define TA 200
#define TB 120
#define TS 40

#define H() GPIO_SetBits(DHT11_PORT, DHT11_PIN)
#define L() GPIO_ResetBits(DHT11_PORT, DHT11_PIN)
#define R() (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) != Bit_RESET)

static void m(GPIOMode_TypeDef x)
{
    GPIO_InitTypeDef i;
    GPIO_StructInit(&i);
    i.GPIO_Pin = DHT11_PIN;
    i.GPIO_Mode = x;
    i.GPIO_PuPd = GPIO_PuPd_UP;
    i.GPIO_Speed = GPIO_Speed_50MHz;
    if (x == GPIO_Mode_OUT)
    {
        i.GPIO_OType = GPIO_OType_OD;
    }
    GPIO_Init(DHT11_PORT, &i);
}

static uint8_t w(uint8_t v, uint32_t t)
{
    while (t--)
    {
        if ((uint8_t)R() == v)
        {
            return 1;
        }
        delay_us(1);
    }
    return 0;
}

static uint8_t g(uint8_t *v)
{
    if (!w(0, TB) || !w(1, TB))
    {
        return 0;
    }
    delay_us(TS);
    *v = (uint8_t)R();
    return w(0, TB);
}

static uint8_t n(uint8_t *v)
{
    uint8_t i, b, x = 0;
    for (i = 0; i < 8; i++)
    {
        if (!g(&b))
        {
            return 0;
        }
        x = (uint8_t)((x << 1) | b);
    }
    *v = x;
    return 1;
}

/* 初始化 DHT11 IO，默认数据脚接 PC6。 */
void DHT11_Init(void)
{
    RCC_AHB1PeriphClockCmd(DHT11_CLK, ENABLE);
    m(GPIO_Mode_IN);
}

/* 读一次温湿度，成功返回 1，t/h 分别是温度和湿度整数值。 */
uint8_t DHT11_Read(int8_t *t, int8_t *h)
{
    uint8_t d[5] = {0};
    uint8_t i, s;
    if (t == 0 || h == 0)
    {
        return 0;
    }
    DHT11_Init();
    m(GPIO_Mode_OUT);
    L();
    delay_ms(DL);
    H();
    delay_us(DR);
    m(GPIO_Mode_IN);
    if (!w(0, TA) || !w(1, TA) || !w(0, TA))
    {
        return 0;
    }
    for (i = 0; i < 5; i++)
    {
        if (!n(&d[i]))
        {
            return 0;
        }
    }
    s = (uint8_t)(d[0] + d[1] + d[2] + d[3]);
    if (s != d[4])
    {
        return 0;
    }
    *h = (int8_t)d[0];
    *t = (int8_t)d[2];
    return 1;
}
