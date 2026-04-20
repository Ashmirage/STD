#ifndef __COUNT_SENSOR_H
#define __COUNT_SENSOR_H
#include "stm32f4xx.h"

#define COUNT_SENSOR_PORT  GPIOB
#define COUNT_SENSOR_PIN   GPIO_PIN_14

void CountSensor_Init(void);
uint16_t CountSensor_Get(void);
void CountSensor_callback(void);


#endif
