#ifndef HCSR04_H
#define HCSR04_H

#include "main.h"
#include "tim.h"

#ifndef HCSR04_TIM
#define HCSR04_TIM htim2
#endif

#ifndef HCSR04_TRIG_GPIO_Port
#define HCSR04_TRIG_GPIO_Port GPIOE
#endif

#ifndef HCSR04_TRIG_Pin
#define HCSR04_TRIG_Pin GPIO_PIN_0
#endif

#ifndef HCSR04_ECHO_GPIO_Port
#define HCSR04_ECHO_GPIO_Port GPIOE
#endif

#ifndef HCSR04_ECHO_Pin
#define HCSR04_ECHO_Pin GPIO_PIN_1
#endif

#ifndef HCSR04_TIMEOUT_US
#define HCSR04_TIMEOUT_US 30000U
#endif

static inline void HCSR04_DelayUs(uint32_t us) {
  uint32_t start_us = __HAL_TIM_GET_COUNTER(&HCSR04_TIM);

  while ((uint32_t)(__HAL_TIM_GET_COUNTER(&HCSR04_TIM) - start_us) < us) {
  }
}

static inline HAL_StatusTypeDef HCSR04_WaitPinState(GPIO_PinState state,
                                                    uint32_t *start_us) {
  *start_us = __HAL_TIM_GET_COUNTER(&HCSR04_TIM);

  while (HAL_GPIO_ReadPin(HCSR04_ECHO_GPIO_Port, HCSR04_ECHO_Pin) == state) {
    if ((uint32_t)(__HAL_TIM_GET_COUNTER(&HCSR04_TIM) - *start_us) >
        HCSR04_TIMEOUT_US) {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

static inline void HCSR04_Init(void) {
  HAL_GPIO_WritePin(HCSR04_TRIG_GPIO_Port, HCSR04_TRIG_Pin, GPIO_PIN_RESET);
  HAL_TIM_Base_Start(&HCSR04_TIM);
}

static inline HAL_StatusTypeDef HCSR04_ReadCm(float *distance_cm) {
  uint32_t start_us;

  if (HCSR04_WaitPinState(GPIO_PIN_SET, &start_us) != HAL_OK) {
    return HAL_TIMEOUT;
  }

  HAL_GPIO_WritePin(HCSR04_TRIG_GPIO_Port, HCSR04_TRIG_Pin, GPIO_PIN_RESET);
  HCSR04_DelayUs(2U);
  HAL_GPIO_WritePin(HCSR04_TRIG_GPIO_Port, HCSR04_TRIG_Pin, GPIO_PIN_SET);
  HCSR04_DelayUs(10U);
  HAL_GPIO_WritePin(HCSR04_TRIG_GPIO_Port, HCSR04_TRIG_Pin, GPIO_PIN_RESET);

  if (HCSR04_WaitPinState(GPIO_PIN_RESET, &start_us) != HAL_OK) {
    return HAL_TIMEOUT;
  }

  if (HCSR04_WaitPinState(GPIO_PIN_SET, &start_us) != HAL_OK) {
    return HAL_TIMEOUT;
  }

  *distance_cm =
      (float)((uint32_t)(__HAL_TIM_GET_COUNTER(&HCSR04_TIM) - start_us)) *
      0.01715f;
  return HAL_OK;
}

#endif
