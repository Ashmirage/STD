#ifndef __REMOTE_H
#define __REMOTE_H

#include "stm32f4xx.h"

/* 红外遥控地址码，当前这套遥控器默认是 0 */
#define REMOTE_ID 0U

extern uint8_t g_remote_cnt;

/* 初始化红外接收头输入捕获，使用 PA8/TIM1_CH1 */
void remote_init(void);

/* 扫描一次红外按键，返回按键码，没按键时返回 0 */
uint8_t remote_scan(void);

/* 定时器更新中断里调用，用来判断一帧数据是否接收完成 */
void Remote_TIM_ElapsedCallback(void);

/* 输入捕获中断里调用，用来解析 NEC 红外脉宽数据 */
void Remote_IC_CaptureCallback(void);

#endif
