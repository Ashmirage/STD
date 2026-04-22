#ifndef __SERVO_H
#define __SERVO_H

/* 初始化舵机 PWM，使用 TIM2_CH2(PA1) 输出 50Hz 控制波形 */
void Servo_Init(void);

/* 设置舵机角度，范围 0~180 度 */
void Servo_SetAngle(float Angle);

#endif
