#ifndef __RTC_CLK_H
#define __RTC_CLK_H

#include <stdint.h>

extern volatile uint16_t My_RTC_time[];

/* 初始化 RTC 时钟域，并把当前时间同步到 My_RTC_time。 */
void RTC_clk_init(void);
/* 把 My_RTC_time 中的时间写入 RTC。 */
void My_RTC_settime(void);
/* 从 RTC 读取当前时间到 My_RTC_time。 */
void My_RTC_readtime(void);

#endif
