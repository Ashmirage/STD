#ifndef __HCSR04_H
#define __HCSR04_H

#include <stdint.h>
#include "stm32f4xx.h"

#ifndef HCSR04_TRIG_PORT
#define HCSR04_TRIG_PORT GPIOE
#endif

#ifndef HCSR04_TRIG_CLK
#define HCSR04_TRIG_CLK RCC_AHB1Periph_GPIOE
#endif

#ifndef HCSR04_TRIG_PIN
#define HCSR04_TRIG_PIN GPIO_Pin_1
#endif

#ifndef HCSR04_ECHO_PORT
#define HCSR04_ECHO_PORT GPIOE
#endif

#ifndef HCSR04_ECHO_CLK
#define HCSR04_ECHO_CLK RCC_AHB1Periph_GPIOE
#endif

#ifndef HCSR04_ECHO_PIN
#define HCSR04_ECHO_PIN GPIO_Pin_3
#endif

#ifndef HCSR04_TIM
#define HCSR04_TIM TIM5
#endif

#ifndef HCSR04_TIM_CLK
#define HCSR04_TIM_CLK RCC_APB1Periph_TIM5
#endif

#ifndef HCSR04_TIMEOUT_US
#define HCSR04_TIMEOUT_US 30000
#endif

void HCSR04_Init(void);
uint8_t HCSR04_Read(float *cm);

#endif
