#ifndef HX711_H
#define HX711_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

#ifndef HX711_DOUT_GPIO_Port
#define HX711_DOUT_GPIO_Port GPIOA
#endif

#ifndef HX711_DOUT_Pin
#define HX711_DOUT_Pin GPIO_PIN_6
#endif

#ifndef HX711_SCK_GPIO_Port
#define HX711_SCK_GPIO_Port GPIOA
#endif

#ifndef HX711_SCK_Pin
#define HX711_SCK_Pin GPIO_PIN_4
#endif

#ifndef HX711_TIMEOUT_MS
#define HX711_TIMEOUT_MS 100U
#endif

#ifndef HX711_DEFAULT_SCALE
#define HX711_DEFAULT_SCALE 1.0f
#endif

typedef struct {
  int32_t offset;
  float scale;
} HX711_HandleTypeDef;

static inline void HX711_Delay(void) {
  volatile uint32_t i;

  for (i = 0; i < 16U; i++) {
  }
}

static inline void HX711_Init(HX711_HandleTypeDef *hx711) {
  hx711->offset = 0;
  hx711->scale = HX711_DEFAULT_SCALE;
  HAL_GPIO_WritePin(HX711_SCK_GPIO_Port, HX711_SCK_Pin, GPIO_PIN_RESET);
}

static inline uint8_t HX711_IsReady(void) {
  return HAL_GPIO_ReadPin(HX711_DOUT_GPIO_Port, HX711_DOUT_Pin) ==
         GPIO_PIN_RESET;
}

static inline HAL_StatusTypeDef HX711_WaitReady(uint32_t timeout_ms) {
  uint32_t start = HAL_GetTick();

  while (!HX711_IsReady()) {
    if ((HAL_GetTick() - start) >= timeout_ms) {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

static inline HAL_StatusTypeDef HX711_ReadRaw(int32_t *data) {
  uint8_t i;
  uint32_t value = 0;

  if (HX711_WaitReady(HX711_TIMEOUT_MS) != HAL_OK) {
    return HAL_TIMEOUT;
  }

  for (i = 0; i < 24U; i++) {
    HAL_GPIO_WritePin(HX711_SCK_GPIO_Port, HX711_SCK_Pin, GPIO_PIN_SET);
    HX711_Delay();
    value = (value << 1) |
            (HAL_GPIO_ReadPin(HX711_DOUT_GPIO_Port, HX711_DOUT_Pin) ==
                     GPIO_PIN_SET
                 ? 1U
                 : 0U);
    HAL_GPIO_WritePin(HX711_SCK_GPIO_Port, HX711_SCK_Pin, GPIO_PIN_RESET);
    HX711_Delay();
  }

  HAL_GPIO_WritePin(HX711_SCK_GPIO_Port, HX711_SCK_Pin, GPIO_PIN_SET);
  HX711_Delay();
  HAL_GPIO_WritePin(HX711_SCK_GPIO_Port, HX711_SCK_Pin, GPIO_PIN_RESET);
  HX711_Delay();

  if (value & 0x800000UL) {
    value |= 0xFF000000UL;
  }

  *data = (int32_t)value;
  return HAL_OK;
}

static inline HAL_StatusTypeDef HX711_ReadAverage(uint8_t samples,
                                                  int32_t *data) {
  uint8_t i;
  int64_t sum = 0;
  int32_t raw = 0;

  if (samples == 0U) {
    samples = 1U;
  }

  for (i = 0; i < samples; i++) {
    if (HX711_ReadRaw(&raw) != HAL_OK) {
      return HAL_TIMEOUT;
    }
    sum += raw;
  }

  *data = (int32_t)(sum / samples);
  return HAL_OK;
}

static inline HAL_StatusTypeDef HX711_Tare(HX711_HandleTypeDef *hx711,
                                           uint8_t samples) {
  return HX711_ReadAverage(samples, &hx711->offset);
}

static inline HAL_StatusTypeDef HX711_GetNet(HX711_HandleTypeDef *hx711,
                                             uint8_t samples,
                                             int32_t *net) {
  int32_t average = 0;

  if (HX711_ReadAverage(samples, &average) != HAL_OK) {
    return HAL_TIMEOUT;
  }

  *net = average - hx711->offset;
  return HAL_OK;
}

static inline HAL_StatusTypeDef HX711_GetWeight(HX711_HandleTypeDef *hx711,
                                                 uint8_t samples,
                                                 float *weight) {
  int32_t net = 0;

  if (HX711_GetNet(hx711, samples, &net) != HAL_OK) {
    return HAL_TIMEOUT;
  }

  *weight = (float)net / hx711->scale;
  return HAL_OK;
}

#endif