#include "Matrix_keyboard.h"
#include "delay.h"

#define MK_DB 10U

static const u16 rpin[4] = {GPIO_Pin_6, GPIO_Pin_4, GPIO_Pin_2, GPIO_Pin_0};
static const u16 cpin[4] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3};

static void rw(u8 i, BitAction s)
{
    GPIO_WriteBit(GPIOE, rpin[i], s);
}

static u8 cr(u8 i)
{
    return (u8)GPIO_ReadInputDataBit(GPIOC, cpin[i]);
}

static u8 scan(void)
{
    u8 r;
    u8 c;
    u8 i;

    for (r = 0; r < 4; r++)
    {
        for (i = 0; i < 4; i++) rw(i, Bit_RESET);
        rw(r, Bit_SET);

        for (c = 0; c < 4; c++)
        {
            if (cr(c))
            {
                rw(r, Bit_RESET);
                return (u8)(r * 4 + c + 1);
            }
        }

        rw(r, Bit_RESET);
    }

    return 0;
}

void MK_Init(void)
{
    GPIO_InitTypeDef io;
    u8 i;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE, ENABLE);

    io.GPIO_Pin = rpin[0] | rpin[1] | rpin[2] | rpin[3];
    io.GPIO_Mode = GPIO_Mode_OUT;
    io.GPIO_OType = GPIO_OType_PP;
    io.GPIO_Speed = GPIO_Speed_50MHz;
    io.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &io);

    io.GPIO_Pin = cpin[0] | cpin[1] | cpin[2] | cpin[3];
    io.GPIO_Mode = GPIO_Mode_IN;
    io.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOC, &io);

    for (i = 0; i < 4; i++) rw(i, Bit_RESET);
}

u8 MK_Get(void)
{
    static u8 st = 0;
    static u8 pre = 0;
    static u8 now = 0;
    static u8 key = 0;
    static u32 t0 = 0;

    pre = now;
    now = scan();

    switch (st)
    {
        case 0:
            if (pre == now && now)
            {
                st = 1;
                t0 = SysTick_GetTick();
            }
            break;

        case 1:
            if (pre != now) st = 0;
            else if ((SysTick_GetTick() - t0) > MK_DB)
            {
                key = now;
                st = 2;
            }
            break;

        case 2:
            if (pre != now || !now)
            {
                st = 0;
                return key;
            }
            break;

        default:
            st = 0;
            break;
    }

    return 0;
}

