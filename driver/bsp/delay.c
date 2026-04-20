#include "delay.h"
#include "sys.h"
#include <stdint.h>

static u8  fac_us = 0;
static u16 fac_ms = 0;

// -------- Minimal addition: ms timestamp for scheduler --------
static u8 dwt_tick_inited = 0;
static u32 dwt_cycles_per_ms = 0;
static u32 dwt_last_cycle = 0;
static uint64_t dwt_cycle_acc = 0;

static void DWT_Init(void)
{
    if (dwt_tick_inited)
    {
        return;
    }

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    dwt_last_cycle = DWT->CYCCNT;
    dwt_cycle_acc = 0;
    dwt_tick_inited = 1;
}

u32 SysTick_GetTick(void)
{
    u32 now;
    u32 delta;

    if ((dwt_cycles_per_ms == 0) || (dwt_tick_inited == 0))
    {
        return 0;
    }

    now = DWT->CYCCNT;
    delta = now - dwt_last_cycle;
    dwt_last_cycle = now;
    dwt_cycle_acc += delta;

    return (u32)(dwt_cycle_acc / dwt_cycles_per_ms);
}


#if SYSTEM_SUPPORT_OS
#include "includes.h"

#ifdef OS_CRITICAL_METHOD
#define delay_osrunning     OSRunning
#define delay_ostickspersec OS_TICKS_PER_SEC
#define delay_osintnesting  OSIntNesting
#endif

#ifdef CPU_CFG_CRITICAL_METHOD
#define delay_osrunning     OSRunning
#define delay_ostickspersec OSCfg_TickRate_Hz
#define delay_osintnesting  OSIntNestingCtr
#endif

void delay_osschedlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD
    OS_ERR err;
    OSSchedLock(&err);
#else
    OSSchedLock();
#endif
}

void delay_osschedunlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD
    OS_ERR err;
    OSSchedUnlock(&err);
#else
    OSSchedUnlock();
#endif
}

void delay_ostimedly(u32 ticks)
{
#ifdef CPU_CFG_CRITICAL_METHOD
    OS_ERR err;
    OSTimeDly(ticks, OS_OPT_TIME_PERIODIC, &err);
#else
    OSTimeDly(ticks);
#endif
}

void SysTick_Handler(void)
{
    if (delay_osrunning == 1)
    {
        OSIntEnter();
        OSTimeTick();
        OSIntExit();
    }
}
#endif

void delay_init(u8 SYSCLK)
{
#if SYSTEM_SUPPORT_OS
    u32 reload;
#endif

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    fac_us = SYSCLK / 8;

#if SYSTEM_SUPPORT_OS
    reload = SYSCLK / 8;
    reload *= 1000000 / delay_ostickspersec;
    fac_ms = 1000 / delay_ostickspersec;
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    SysTick->LOAD = reload;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
#else
    fac_ms = (u16)fac_us * 1000;
#endif

    dwt_cycles_per_ms = (u32)SYSCLK * 1000U;
    DWT_Init();
}

#if SYSTEM_SUPPORT_OS
void delay_us(u32 nus)
{
    u32 ticks;
    u32 told;
    u32 tnow;
    u32 tcnt = 0;
    u32 reload = SysTick->LOAD;

    ticks = nus * fac_us;
    delay_osschedlock();
    told = SysTick->VAL;

    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }

            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }

    delay_osschedunlock();
}

void delay_ms(u16 nms)
{
    if (delay_osrunning && delay_osintnesting == 0)
    {
        if (nms >= fac_ms)
        {
            delay_ostimedly(nms / fac_ms);
        }
        nms %= fac_ms;
    }

    delay_us((u32)(nms * 1000));
}

#else
void delay_us(u32 nus)
{
    u32 temp;

    SysTick->LOAD = nus * fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16)));

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0X00;
}

static void delay_xms(u16 nms)
{
    u32 temp;

    SysTick->LOAD = (u32)nms * fac_ms;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16)));

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0X00;
}

void delay_ms(u16 nms)
{
    u8 repeat = nms / 540;
    u16 remain = nms % 540;

    while (repeat)
    {
        delay_xms(540);
        repeat--;
    }

    if (remain)
    {
        delay_xms(remain);
    }
}
#endif
