#ifndef MPU6050_H
#define MPU6050_H

#include "main.h"
#include <stdint.h>

#ifndef MPU6050_SCL_GPIO_Port
#define MPU6050_SCL_GPIO_Port GPIOB
#endif

#ifndef MPU6050_SCL_Pin
#define MPU6050_SCL_Pin GPIO_PIN_6
#endif

#ifndef MPU6050_SDA_GPIO_Port
#define MPU6050_SDA_GPIO_Port GPIOB
#endif

#ifndef MPU6050_SDA_Pin
#define MPU6050_SDA_Pin GPIO_PIN_7
#endif

#ifndef MPU6050_ADDR
#define MPU6050_ADDR 0x68U
#endif

#define MPU6050_REG_ACCEL_XOUT_H 0x3BU
#define MPU6050_REG_PWR_MGMT_1   0x6BU
#define MPU6050_REG_WHO_AM_I     0x75U

#define MPU6050_SCL(x) HAL_GPIO_WritePin(MPU6050_SCL_GPIO_Port, MPU6050_SCL_Pin, (x))
#define MPU6050_SDA(x) HAL_GPIO_WritePin(MPU6050_SDA_GPIO_Port, MPU6050_SDA_Pin, (x))
#define MPU6050_SDA_READ() HAL_GPIO_ReadPin(MPU6050_SDA_GPIO_Port, MPU6050_SDA_Pin)

typedef struct {
  int16_t ax;
  int16_t ay;
  int16_t az;
  int16_t temp;
  int16_t gx;
  int16_t gy;
  int16_t gz;
} MPU6050_DataTypeDef;

static inline void MPU6050_Delay(void) {
  volatile uint32_t i;

  for (i = 0; i < 32U; i++) {
  }
}

static inline void MPU6050_Start(void) {
  MPU6050_SDA(GPIO_PIN_SET);
  MPU6050_SCL(GPIO_PIN_SET);
  MPU6050_Delay();
  MPU6050_SDA(GPIO_PIN_RESET);
  MPU6050_Delay();
  MPU6050_SCL(GPIO_PIN_RESET);
}

static inline void MPU6050_Stop(void) {
  MPU6050_SDA(GPIO_PIN_RESET);
  MPU6050_Delay();
  MPU6050_SCL(GPIO_PIN_SET);
  MPU6050_Delay();
  MPU6050_SDA(GPIO_PIN_SET);
  MPU6050_Delay();
}

static inline HAL_StatusTypeDef MPU6050_WriteByte(uint8_t byte) {
  uint8_t i;

  for (i = 0; i < 8U; i++) {
    MPU6050_SDA((byte & 0x80U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    MPU6050_Delay();
    MPU6050_SCL(GPIO_PIN_SET);
    MPU6050_Delay();
    MPU6050_SCL(GPIO_PIN_RESET);
    byte <<= 1;
  }

  MPU6050_SDA(GPIO_PIN_SET);
  MPU6050_Delay();
  MPU6050_SCL(GPIO_PIN_SET);
  MPU6050_Delay();
  if (MPU6050_SDA_READ() == GPIO_PIN_SET) {
    MPU6050_SCL(GPIO_PIN_RESET);
    return HAL_ERROR;
  }
  MPU6050_SCL(GPIO_PIN_RESET);
  return HAL_OK;
}

static inline uint8_t MPU6050_ReadByte(GPIO_PinState nack) {
  uint8_t i;
  uint8_t byte = 0U;

  MPU6050_SDA(GPIO_PIN_SET);
  for (i = 0; i < 8U; i++) {
    byte <<= 1;
    MPU6050_SCL(GPIO_PIN_SET);
    MPU6050_Delay();
    if (MPU6050_SDA_READ() == GPIO_PIN_SET) {
      byte |= 1U;
    }
    MPU6050_SCL(GPIO_PIN_RESET);
    MPU6050_Delay();
  }

  MPU6050_SDA(nack);
  MPU6050_Delay();
  MPU6050_SCL(GPIO_PIN_SET);
  MPU6050_Delay();
  MPU6050_SCL(GPIO_PIN_RESET);
  MPU6050_SDA(GPIO_PIN_SET);
  return byte;
}

static inline HAL_StatusTypeDef MPU6050_WriteReg(uint8_t reg, uint8_t value) {
  MPU6050_Start();
  if (MPU6050_WriteByte((uint8_t)(MPU6050_ADDR << 1)) != HAL_OK ||
      MPU6050_WriteByte(reg) != HAL_OK || MPU6050_WriteByte(value) != HAL_OK) {
    MPU6050_Stop();
    return HAL_ERROR;
  }
  MPU6050_Stop();
  return HAL_OK;
}

static inline HAL_StatusTypeDef MPU6050_ReadRegs(uint8_t reg, uint8_t *buf,
                                                 uint8_t len) {
  uint8_t i;

  MPU6050_Start();
  if (MPU6050_WriteByte((uint8_t)(MPU6050_ADDR << 1)) != HAL_OK ||
      MPU6050_WriteByte(reg) != HAL_OK) {
    MPU6050_Stop();
    return HAL_ERROR;
  }

  MPU6050_Start();
  if (MPU6050_WriteByte((uint8_t)((MPU6050_ADDR << 1) | 0x01U)) != HAL_OK) {
    MPU6050_Stop();
    return HAL_ERROR;
  }

  for (i = 0; i < len; i++) {
    buf[i] = MPU6050_ReadByte((i == (uint8_t)(len - 1U)) ? GPIO_PIN_SET
                                                         : GPIO_PIN_RESET);
  }

  MPU6050_Stop();
  return HAL_OK;
}

static inline HAL_StatusTypeDef MPU6050_Init(void) {
  uint8_t who_am_i = 0U;

  if (MPU6050_ReadRegs(MPU6050_REG_WHO_AM_I, &who_am_i, 1U) != HAL_OK) {
    return HAL_ERROR;
  }

  if (who_am_i != MPU6050_ADDR) {
    return HAL_ERROR;
  }

  if (MPU6050_WriteReg(MPU6050_REG_PWR_MGMT_1, 0x00U) != HAL_OK) {
    return HAL_ERROR;
  }

  HAL_Delay(10);
  return HAL_OK;
}

static inline HAL_StatusTypeDef MPU6050_ReadAll(MPU6050_DataTypeDef *data) {
  uint8_t buf[14];

  if (MPU6050_ReadRegs(MPU6050_REG_ACCEL_XOUT_H, buf, 14U) != HAL_OK) {
    return HAL_ERROR;
  }

  data->ax = (int16_t)((buf[0] << 8) | buf[1]);
  data->ay = (int16_t)((buf[2] << 8) | buf[3]);
  data->az = (int16_t)((buf[4] << 8) | buf[5]);
  data->temp = (int16_t)((buf[6] << 8) | buf[7]);
  data->gx = (int16_t)((buf[8] << 8) | buf[9]);
  data->gy = (int16_t)((buf[10] << 8) | buf[11]);
  data->gz = (int16_t)((buf[12] << 8) | buf[13]);
  return HAL_OK;
}

static inline float MPU6050_TempC(const MPU6050_DataTypeDef *data) {
  return (float)data->temp / 340.0f + 36.53f;
}

static inline void MPU6050_AccelG(const MPU6050_DataTypeDef *data, float *ax,
                                  float *ay, float *az) {
  *ax = (float)data->ax / 16384.0f;
  *ay = (float)data->ay / 16384.0f;
  *az = (float)data->az / 16384.0f;
}

static inline void MPU6050_GyroDps(const MPU6050_DataTypeDef *data, float *gx,
                                   float *gy, float *gz) {
  *gx = (float)data->gx / 131.0f;
  *gy = (float)data->gy / 131.0f;
  *gz = (float)data->gz / 131.0f;
}

#endif
