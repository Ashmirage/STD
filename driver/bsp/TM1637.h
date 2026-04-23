#ifndef __TM1637_H
#define __TM1637_H

#include "stm32f4xx.h"

#ifndef TM1637_CLK_PORT
#define TM1637_CLK_PORT GPIOD
#endif

#ifndef TM1637_CLK_RCC
#define TM1637_CLK_RCC RCC_AHB1Periph_GPIOD
#endif

#ifndef TM1637_CLK_PIN
#define TM1637_CLK_PIN GPIO_Pin_1
#endif

#ifndef TM1637_DIO_PORT
#define TM1637_DIO_PORT GPIOD
#endif

#ifndef TM1637_DIO_RCC
#define TM1637_DIO_RCC RCC_AHB1Periph_GPIOD
#endif

#ifndef TM1637_DIO_PIN
#define TM1637_DIO_PIN GPIO_Pin_2
#endif

extern const uint8_t TM1637_NUM[10];

void TM1637_Init(void);
void TM1637_Display(const uint8_t d[4]);

#endif
