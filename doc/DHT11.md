# DHT11

## 1. 接线说明

- `DATA -> PC6`
- `VCC -> 3.3V`
- `GND -> GND`

公开接口：

```c
void DHT11_Init(void);
uint8_t DHT11_Read(int8_t *t, int8_t *h);
```

`DHT11_Read()` 成功返回 `1`，失败返回 `0`。

## 2. 调用示例

```c
#include "stm32f4xx.h"
#include "delay.h"
#include "DHT11.h"

int main(void)
{
    int8_t t = 0;
    int8_t h = 0;

    SystemCoreClockUpdate();
    delay_init((u8)(SystemCoreClock / 1000000));
    DHT11_Init();

    while (1)
    {
        DHT11_Read(&t, &h);
        delay_ms(1000);
    }
}
```

## 3.1 当前工程接入位置

当前工程已经在 `Hardware_init()` 里初始化：

```c
void Hardware_init(void)
{
    uart_init(115200);
    // 其他初始化...

    DHT11_Init();
}
```

读取示例：

```c
int8_t t = 0;
int8_t h = 0;

if (DHT11_Read(&t, &h))
{
    Send_printf("t=%d h=%d\r\n", t, h);
}
```

## 3.2 更改引脚注意事项

- 当前这份代码已经把引脚做成宏了，优先改 `driver/bsp/DHT11.h`
  - `DHT11_PORT`
  - `DHT11_CLK`
  - `DHT11_PIN`
- `DHT11.c` 里所有 IO 访问都走这 3 个宏，所以如果只是换普通 GPIO，不需要再去改 `.c` 文件逻辑
- 这份实现要求数据脚能在“输入上拉”和“开漏输出”之间切换，所以不要改成推挽输出
- 如果你用的是裸 `DHT11`，上拉电阻别省
- 这份代码依赖 `delay_us()` 和 `delay_ms()`，比赛时如果 `delay` 没先配好，DHT11 一定读不出来

## 4. 完整驱动代码

### DHT11.h

```c
#ifndef __DHT11_H
#define __DHT11_H

#include <stdint.h>
#include "stm32f4xx.h"

#ifndef DHT11_PORT
#define DHT11_PORT GPIOC
#endif

#ifndef DHT11_CLK
#define DHT11_CLK RCC_AHB1Periph_GPIOC
#endif

#ifndef DHT11_PIN
#define DHT11_PIN GPIO_Pin_6
#endif

void DHT11_Init(void);
uint8_t DHT11_Read(int8_t *t, int8_t *h);

#endif
```

### DHT11.c

```c
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

void DHT11_Init(void)
{
    RCC_AHB1PeriphClockCmd(DHT11_CLK, ENABLE);
    m(GPIO_Mode_IN);
}

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
```
