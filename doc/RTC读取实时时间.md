- 模块：RTC 读取实时时间

- 当前资源：
  - 软件里没有占用普通 GPIO
  - 当前代码使用 `LSE` 作为 RTC 时钟源

- 使用方法：
  ```c
  #include "RTC_clk.h"

  RTC_clk_init();
  ```

  ```c
  My_RTC_readtime();
  Send_printf("%d-%d-%d %d-%d-%d\r\n",
              My_RTC_time[0], My_RTC_time[1], My_RTC_time[2],
              My_RTC_time[3], My_RTC_time[4], My_RTC_time[5]);
  ```

- 默认时间：
  ```c
  volatile uint16_t My_RTC_time[] = {2026, 4, 21, 8, 47, 0};
  ```

- 说明：
  - 第一次上电时，会把 `My_RTC_time` 里的默认时间写进 RTC
  - 后面掉电保持依赖备份域和 RTC 时钟源

## 当前工程接入位置

当前工程已经在 `Hardware_init()` 里初始化：

```c
void Hardware_init(void)
{
    uart_init(115200);
    // 其他初始化...

    RTC_clk_init();
}
```

读取时间的示例，当前工程里也已经留好了：

```c
My_RTC_readtime();
Send_printf("%d-%d-%d %d-%d-%d\r\n",
            My_RTC_time[0], My_RTC_time[1], My_RTC_time[2],
            My_RTC_time[3], My_RTC_time[4], My_RTC_time[5]);
```

## 更改时钟源/引脚时要注意

- 这份代码不是通过普通 GPIO 初始化 RTC，所以没有“改一个 `GPIO_Pin_x` 宏就结束”这么简单
- 当前 `RTC_clk.c` 是固定按 `LSE = 32.768kHz` 写的：
  - `RCC_LSEConfig(RCC_LSE_ON);`
  - `RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);`
  - `RTC_AsynchPrediv = 127`
  - `RTC_SynchPrediv = 255`

```c
volatile uint16_t My_RTC_time[] = {2026, 4, 21, 8, 47, 0};
```

## 比赛直接抄的完整代码

### RTC_clk.h

```c
#ifndef __RTC_CLK_H
#define __RTC_CLK_H

#include <stdint.h>

extern volatile uint16_t My_RTC_time[];

/* 初始化 RTC，并把当前时间同步到 My_RTC_time。 */
void RTC_clk_init(void);
/* 把 My_RTC_time 里的时间写进 RTC。 */
void My_RTC_settime(void);
/* 从 RTC 读取当前时间到 My_RTC_time。 */
void My_RTC_readtime(void);

#endif
```

### RTC_clk.c

```c
#include "stm32f4xx.h"
#include "RTC_clk.h"

#define RTC_FLAG 0x6666U

volatile uint16_t My_RTC_time[] = {2026, 4, 21, 8, 47, 0};

static uint8_t rt_ok = 0;

/* 根据年月日算星期，给 RTC 用。 */
static uint8_t rtc_wd(uint16_t y, uint8_t m, uint8_t d)
{
    static const uint8_t t[] = {0U, 3U, 2U, 5U, 0U, 3U, 5U, 1U, 4U, 6U, 2U, 4U};

    if (m < 3U)
    {
        y--;
    }

    d = (uint8_t)((y + y / 4U - y / 100U + y / 400U + t[m - 1U] + d) % 7U);
    return (d == 0U) ? RTC_Weekday_Sunday : d;
}

/* RTC 基本配置，LSE 下 1Hz。 */
static void rtc_cfg(void)
{
    RTC_InitTypeDef i;

    i.RTC_HourFormat = RTC_HourFormat_24;
    i.RTC_AsynchPrediv = 127U;
    i.RTC_SynchPrediv = 255U;
    RTC_Init(&i);
}

/* 把 My_RTC_time 写进 RTC。 */
static void rtc_wr(void)
{
    RTC_TimeTypeDef t;
    RTC_DateTypeDef d;

    t.RTC_H12 = RTC_H12_AM;
    t.RTC_Hours = (uint8_t)My_RTC_time[3];
    t.RTC_Minutes = (uint8_t)My_RTC_time[4];
    t.RTC_Seconds = (uint8_t)My_RTC_time[5];
    RTC_SetTime(RTC_Format_BIN, &t);

    d.RTC_Year = (uint8_t)(My_RTC_time[0] >= 2000U ? My_RTC_time[0] - 2000U : My_RTC_time[0]);
    d.RTC_Month = (uint8_t)My_RTC_time[1];
    d.RTC_Date = (uint8_t)My_RTC_time[2];
    d.RTC_WeekDay = rtc_wd(My_RTC_time[0], (uint8_t)My_RTC_time[1], (uint8_t)My_RTC_time[2]);
    RTC_SetDate(RTC_Format_BIN, &d);

    RTC_WriteBackupRegister(RTC_BKP_DR0, RTC_FLAG);
}

/* 从 RTC 读当前时间到 My_RTC_time。 */
static void rtc_rd(void)
{
    RTC_TimeTypeDef t;
    RTC_DateTypeDef d;

    RTC_GetTime(RTC_Format_BIN, &t);
    RTC_GetDate(RTC_Format_BIN, &d);

    My_RTC_time[0] = (uint16_t)(d.RTC_Year + 2000U);
    My_RTC_time[1] = d.RTC_Month;
    My_RTC_time[2] = d.RTC_Date;
    My_RTC_time[3] = t.RTC_Hours;
    My_RTC_time[4] = t.RTC_Minutes;
    My_RTC_time[5] = t.RTC_Seconds;
}

/* 初始化 RTC/LSE，只做一次。 */
void RTC_clk_init(void)
{
    if (rt_ok != 0U)
    {
        return;
    }

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    if (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
        RCC_LSEConfig(RCC_LSE_ON);
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
        {
        }
    }

    if ((RCC->BDCR & RCC_BDCR_RTCSEL) == 0U)
    {
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    }

    RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForSynchro();
    rtc_cfg();

    if (RTC_ReadBackupRegister(RTC_BKP_DR0) != RTC_FLAG)
    {
        rtc_wr();
    }

    rtc_rd();
    rt_ok = 1U;
}

/* 把 My_RTC_time 里的值写进 RTC。 */
void My_RTC_settime(void)
{
    RTC_clk_init();
    rtc_wr();
}

/* 从 RTC 读取当前时间到 My_RTC_time。 */
void My_RTC_readtime(void)
{
    RTC_clk_init();
    rtc_rd();
}
```
