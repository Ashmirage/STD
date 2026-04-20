#ifndef __TM1637_H__
#define __TM1637_H__

#include "main.h"

#define TM1637_CLK(x) HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, x)
#define TM1637_DIO(x) HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, x)

static const uint8_t TM1637_NUM[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66,
                                       0x6D, 0x7D, 0x07, 0x7F, 0x6F};

static inline void TM1637_Start(void) {
  TM1637_DIO(GPIO_PIN_SET);
  TM1637_CLK(GPIO_PIN_SET);
  TM1637_DIO(GPIO_PIN_RESET);
  TM1637_CLK(GPIO_PIN_RESET);
}

static inline void TM1637_Stop(void) {
  TM1637_CLK(GPIO_PIN_RESET);
  TM1637_DIO(GPIO_PIN_RESET);
  TM1637_CLK(GPIO_PIN_SET);
  TM1637_DIO(GPIO_PIN_SET);
}

static inline void TM1637_WriteByte(uint8_t data) {
  uint8_t i;

  for (i = 0; i < 8; i++) {
    TM1637_CLK(GPIO_PIN_RESET);
    TM1637_DIO((GPIO_PinState)(data & 0x01U));
    data >>= 1;
    TM1637_CLK(GPIO_PIN_SET);
  }

  TM1637_CLK(GPIO_PIN_RESET);
  TM1637_DIO(GPIO_PIN_SET);
  TM1637_CLK(GPIO_PIN_SET);
}

static inline void TM1637_Init(void) {
  TM1637_CLK(GPIO_PIN_SET);
  TM1637_DIO(GPIO_PIN_SET);
}

static inline void TM1637_Display(const uint8_t data[4]) {
  uint8_t i;

  TM1637_Start();
  TM1637_WriteByte(0x40);
  TM1637_Stop();

  TM1637_Start();
  TM1637_WriteByte(0xC0);
  for (i = 0; i < 4; i++) {
    TM1637_WriteByte(data[i]);
  }
  TM1637_Stop();

  TM1637_Start();
  TM1637_WriteByte(0x8F);
  TM1637_Stop();
}

#endif
