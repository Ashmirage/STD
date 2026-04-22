# HC-SR04

## 1. 接线说明

- `TRIG -> PE1`
- `ECHO -> PE3`
- `VCC -> 5V`
- `GND -> GND`

很多 `HC-SR04` 模块的 `ECHO` 是 `5V` 输出，接 STM32 前建议做分压或电平转换。

当前主工程默认改成这组资源：

- IO 改成 `PE1 + PE3`
- 定时器改成 `TIM5`

公开接口：

```c
void HCSR04_Init(void);
uint8_t HCSR04_Read(float *cm);
```

`HCSR04_Read()` 成功返回 `1`，失败返回 `0`。

## 2. 调用示例

```c
#include "stm32f4xx.h"
#include "delay.h"
#include "HCSR04.h"

int main(void)
{
    float cm = 0.0f;

    SystemCoreClockUpdate();
    delay_init((u8)(SystemCoreClock / 1000000));
    HCSR04_Init();

    while (1)
    {
        HCSR04_Read(&cm);
        delay_ms(60);
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

    HCSR04_Init();
}
```

读取示例：

```c
float distance = 0.0f;

if (HCSR04_Read(&distance))
{
    Send_printf("distance=%.3f\r\n", distance);
}
```

## 3.2 更改引脚/定时器注意事项

- 如果只是改 `TRIG/ECHO` 的 GPIO，优先改 `driver/bsp/HCSR04.h`
  - `HCSR04_TRIG_PORT`
  - `HCSR04_TRIG_CLK`
  - `HCSR04_TRIG_PIN`
  - `HCSR04_ECHO_PORT`
  - `HCSR04_ECHO_CLK`
  - `HCSR04_ECHO_PIN`
- 如果还是用 `APB1` 上的定时器，比如 `TIM2/TIM3/TIM4/TIM5`，也可以只改：
  - `HCSR04_TIM`
  - `HCSR04_TIM_CLK`
- 但如果你换成 `TIM1/TIM8` 这种 `APB2` 定时器，不能只改宏，因为当前 `HCSR04.c` 里写死的是：

```c
RCC_APB1PeriphClockCmd(HCSR04_TIM_CLK, ENABLE);
```

- 这时还要把上面这一句一起改成对应的 `RCC_APB2PeriphClockCmd(...)`
- 当前代码选 `TIM5` 的一个实际好处是它是 `32bit` 定时器，量程更宽，不容易溢出
- 很多 `HC-SR04` 的 `ECHO` 是 `5V`，这点不处理好，比代码更容易出问题

## 4. 完整驱动代码

### HCSR04.h

```c
#ifndef __HCSR04_H
#define __HCSR04_H

#include <stdint.h>
#include "stm32f4xx.h"

#ifndef HCSR04_TRIG_PORT
#define HCSR04_TRIG_PORT GPIOE
#endif

#ifndef HCSR04_TRIG_CLK
#define HCSR04_TRIG_CLK RCC_AHB1Periph_GPIOE
#endif

#ifndef HCSR04_TRIG_PIN
#define HCSR04_TRIG_PIN GPIO_Pin_1
#endif

#ifndef HCSR04_ECHO_PORT
#define HCSR04_ECHO_PORT GPIOE
#endif

#ifndef HCSR04_ECHO_CLK
#define HCSR04_ECHO_CLK RCC_AHB1Periph_GPIOE
#endif

#ifndef HCSR04_ECHO_PIN
#define HCSR04_ECHO_PIN GPIO_Pin_3
#endif

#ifndef HCSR04_TIM
#define HCSR04_TIM TIM5
#endif

#ifndef HCSR04_TIM_CLK
#define HCSR04_TIM_CLK RCC_APB1Periph_TIM5
#endif

#ifndef HCSR04_TIMEOUT_US
#define HCSR04_TIMEOUT_US 30000
#endif

void HCSR04_Init(void);
uint8_t HCSR04_Read(float *cm);

#endif
```

### HCSR04.c

```c
#include "HCSR04.h"
#include "delay.h"

#define TIMX HCSR04_TIM
#define T0() GPIO_ResetBits(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN)
#define T1() GPIO_SetBits(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN)
#define E()  (GPIO_ReadInputDataBit(HCSR04_ECHO_PORT, HCSR04_ECHO_PIN) == Bit_SET)

static uint8_t x = 0;

static uint32_t c(void)
{
    RCC_ClocksTypeDef r;
    RCC_GetClocksFreq(&r);
    return ((RCC->CFGR & RCC_CFGR_PPRE1) == RCC_CFGR_PPRE1_DIV1) ? r.PCLK1_Frequency : (r.PCLK1_Frequency * 2);
}

static void i(void)
{
    GPIO_InitTypeDef g;
    TIM_TimeBaseInitTypeDef t;

    RCC_AHB1PeriphClockCmd(HCSR04_TRIG_CLK | HCSR04_ECHO_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(HCSR04_TIM_CLK, ENABLE);

    GPIO_StructInit(&g);
    g.GPIO_Pin = HCSR04_TRIG_PIN;
    g.GPIO_Mode = GPIO_Mode_OUT;
    g.GPIO_OType = GPIO_OType_PP;
    g.GPIO_PuPd = GPIO_PuPd_NOPULL;
    g.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HCSR04_TRIG_PORT, &g);

    g.GPIO_Pin = HCSR04_ECHO_PIN;
    g.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(HCSR04_ECHO_PORT, &g);

    T0();
    TIM_TimeBaseStructInit(&t);
    t.TIM_Prescaler = (uint16_t)(c() / 1000000 - 1);
    t.TIM_CounterMode = TIM_CounterMode_Up;
    t.TIM_Period = 0xFFFFFFFF;
    t.TIM_ClockDivision = TIM_CKD_DIV1;
    t.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIMX, &t);
    TIM_SetCounter(TIMX, 0);
    TIM_Cmd(TIMX, ENABLE);
    x = 1;
}

static uint8_t w(uint8_t v)
{
    uint32_t s = TIM_GetCounter(TIMX);
    while ((uint8_t)E() != v)
    {
        if ((uint32_t)(TIM_GetCounter(TIMX) - s) >= HCSR04_TIMEOUT_US)
        {
            return 0;
        }
    }
    return 1;
}

void HCSR04_Init(void)
{
    i();
}

uint8_t HCSR04_Read(float *cm)
{
    uint32_t s;
    if (cm == 0)
    {
        return 0;
    }
    if (!x)
    {
        HCSR04_Init();
    }
    if (!w(0))
    {
        return 0;
    }
    T0();
    delay_us(2);
    T1();
    delay_us(10);
    T0();
    if (!w(1))
    {
        return 0;
    }
    s = TIM_GetCounter(TIMX);
    if (!w(0))
    {
        return 0;
    }
    *cm = (float)(TIM_GetCounter(TIMX) - s) * 0.01715f;
    return 1;
}
```
