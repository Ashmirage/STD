- 把 system_stm32f4xx.c (line 316) 里的 PLL_M 从 25 改成 8

- 修改c/c++宏定义,新增加一个宏 HSE_VALUE=8000000

- 在delay.c里面新增毫秒时间戳,用于任务调度器:
  ```c
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
  
  
  ```

  delay.h里面声明`SysTick_GetTick`