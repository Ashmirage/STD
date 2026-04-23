#include "TM1637.h"

#define C(x) GPIO_WriteBit(TM1637_CLK_PORT, TM1637_CLK_PIN, (BitAction)(x))
#define D(x) GPIO_WriteBit(TM1637_DIO_PORT, TM1637_DIO_PIN, (BitAction)(x))

const uint8_t TM1637_NUM[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

static void o(GPIO_TypeDef *p, uint16_t n)
{
    GPIO_InitTypeDef i;
    GPIO_StructInit(&i);
    i.GPIO_Pin = n;
    i.GPIO_Mode = GPIO_Mode_OUT;
    i.GPIO_OType = GPIO_OType_PP;
    i.GPIO_PuPd = GPIO_PuPd_UP;
    i.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(p, &i);
}

static void s(void)
{
    D(1);
    C(1);
    D(0);
    C(0);
}

static void p(void)
{
    C(0);
    D(0);
    C(1);
    D(1);
}

static void w(uint8_t x)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        C(0);
        D(x & 1);
        x >>= 1;
        C(1);
    }
    C(0);
    D(1);
    C(1);
}

void TM1637_Init(void)
{
    RCC_AHB1PeriphClockCmd(TM1637_CLK_RCC | TM1637_DIO_RCC, ENABLE);
    o(TM1637_CLK_PORT, TM1637_CLK_PIN);
    o(TM1637_DIO_PORT, TM1637_DIO_PIN);
    C(1);
    D(1);
}

void TM1637_Display(const uint8_t d[4])
{
    uint8_t i;
    s();
    w(0x40);
    p();
    s();
    w(0xc0);
    for (i = 0; i < 4; i++)
    {
        w(d[i]);
    }
    p();
    s();
    w(0x8f);
    p();
}
