#ifndef __SERVO_H
#define __SERVO_H

/* 初始化舵机 PWM：PA1 输出 TIM2_CH2，频率 50Hz */
void Servo_Init(void);

/* 设置舵机角度：0~180 度 */
void Servo_SetAngle(float angle);

#endif
