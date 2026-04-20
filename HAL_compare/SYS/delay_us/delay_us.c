#include "delay_us.h"
#include "core_cm4.h"  // STM32F4 系列 Cortex-M4

static uint32_t last_tick = 0;
static uint64_t us_accum = 0;

void DWT_Init(void) {
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void delay_us(uint32_t us) {
    uint32_t startTick = DWT->CYCCNT;
    uint32_t delayTicks = us * (HAL_RCC_GetHCLKFreq() / 1000000);
    while ((DWT->CYCCNT - startTick) < delayTicks);
}


/* 微秒级时间戳，返回累积值，不会因为 32 位溢出出错 */
uint64_t get_us_timestamp(void)
{
    uint32_t now = DWT->CYCCNT;
    if (now < last_tick) {
        /* CYCCNT 溢出时累加 2^32 个周期 */
        us_accum += ((uint64_t)0xFFFFFFFF + 1) / (HAL_RCC_GetHCLKFreq() / 1000000);
    }
    last_tick = now;

    return us_accum + (now / (HAL_RCC_GetHCLKFreq() / 1000000));
}



