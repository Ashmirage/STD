#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdint.h>

/* 初始化直流电机引脚和 PWM。 */
void Motor_init(void);

/* 设置电机速度，范围 -100~100，正负代表方向。 */
void Motor_set_speed(int8_t speed);

/* 获取电机当前是否处于运行状态，0=停止，1=运行。 */
uint8_t Motor_get_status(void);

#endif
