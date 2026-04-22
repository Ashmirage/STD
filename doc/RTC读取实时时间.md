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

