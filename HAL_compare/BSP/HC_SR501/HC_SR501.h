#ifndef HC_SR501_H
#define HC_SR501_H

#include "stm32f4xx_hal.h"

#ifndef HC_SR501_OUT_GPIO_Port
#define HC_SR501_OUT_GPIO_Port GPIOC
#endif

#ifndef HC_SR501_OUT_Pin
#define HC_SR501_OUT_Pin GPIO_PIN_2
#endif

static inline uint8_t HC_SR501_Detected(void) {
  return HAL_GPIO_ReadPin(HC_SR501_OUT_GPIO_Port, HC_SR501_OUT_Pin) ==
         GPIO_PIN_SET;
}

#endif