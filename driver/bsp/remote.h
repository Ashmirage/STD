#ifndef __REMOTE_H
#define __REMOTE_H

#include "stm32f4xx.h"

/* 当前遥控器地址码默认是 0。 */
#define REMOTE_ID 0U

extern uint8_t g_remote_cnt;

/* 红外接收头初始化，当前用 PA8/TIM1_CH1。 */
void remote_init(void);

/* 扫描一次按键，没按到返回 0。 */
uint8_t remote_scan(void);

/* 更新中断里调用。 */
void Remote_TIM_ElapsedCallback(void);

/* 捕获中断里调用，用来解析 NEC 脉宽。 */
void Remote_IC_CaptureCallback(void);

#endif
