#ifndef __MQ2_H
#define	__MQ2_H
#include "stm32f10x.h"



#define MQ2_READ_TIMES	10  

#define	MODE 	1

#if MODE
#define 	MQ2_AO_GPIO_PORT							GPIOC
#define 	MQ2_AO_GPIO_PIN								GPIO_PIN_5

#else
#define 	MQ2_DO_GPIO_PORT							GPIOA
#define 	MQ2_DO_GPIO_PIN								GPIO_PIN_8

#endif
/*********************END**********************/

extern uint16_t mq2_data;


void MQ2_Init(void);
uint16_t MQ2_GetData(void);
float MQ2_GetData_PPM(void);

#endif /* __ADC_H */


