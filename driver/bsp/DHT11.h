#ifndef __DHT11_H
#define __DHT11_H

#include <stdint.h>
#include "stm32f4xx.h"

#ifndef DHT11_PORT
#define DHT11_PORT GPIOC
#endif

#ifndef DHT11_CLK
#define DHT11_CLK RCC_AHB1Periph_GPIOC
#endif

#ifndef DHT11_PIN
#define DHT11_PIN GPIO_Pin_6
#endif

void DHT11_Init(void);
uint8_t DHT11_Read(int8_t *t, int8_t *h);

#endif
