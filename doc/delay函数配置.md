- 把 system_stm32f4xx.c (line 316) 里的 PLL_M 从 25 改成 8

- 修改c/c++宏定义,新增加一个宏 HSE_VALUE=8000000

- 在delay.c里面新增毫秒时间戳,用于任务调度器:
  ```c
  // 使用 DWT 周期计数器提供调度器需要的 ms 时间戳
  static u8 ok = 0;
  static u32 ms = 0;        // 每毫秒对应的 CPU 周期数
  static u32 last = 0;      // 上一次读取到的 CYCCNT
  static uint64_t acc = 0;  // 已累计的 CPU 周期数
  
  static void DWT_Init(void)
  {
      if (ok) return;
  
      // 使能 DWT CYCCNT 周期计数器
      CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
      DWT->CYCCNT = 0;
      DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  
      last = DWT->CYCCNT;
      acc = 0;
      ok = 1;
  }
  
  u32 SysTick_GetTick(void)
  {
      u32 now;
      u32 d;
  
      if ((ms == 0) || (ok == 0)) return 0;
  
      // 累加两次调用之间经过的周期数，再换算成 ms
      now = DWT->CYCCNT;
      d = now - last;
      last = now;
      acc += d;
  
      return (u32)(acc / ms);
  }
  
  
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
  
      ms = (u32)SYSCLK * 1000U;  // SYSCLK 单位是 MHz，MHz * 1000 = cycles/ms
  	DWT_Init();
  }
  
  
  ```
  
  delay.h里面声明`SysTick_GetTick`