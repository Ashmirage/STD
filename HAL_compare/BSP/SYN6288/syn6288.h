#ifndef __SYN6288_H
#define __SYN6288_H

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SYN6288_CONTROL_FRAME_LEN 5U
#define SYN6288_TX_TIMEOUT_MS     1000U
#define SYN6288_MAX_TEXT_BYTES    200U

extern const uint8_t SYN_StopCom[SYN6288_CONTROL_FRAME_LEN];
extern const uint8_t SYN_SuspendCom[SYN6288_CONTROL_FRAME_LEN];
extern const uint8_t SYN_RecoverCom[SYN6288_CONTROL_FRAME_LEN];
extern const uint8_t SYN_ChackCom[SYN6288_CONTROL_FRAME_LEN];
extern const uint8_t SYN_PowerDownCom[SYN6288_CONTROL_FRAME_LEN];

void SYN6288_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef SYN_FrameInfoUtf8(uint8_t music, const char *utf8_text);
HAL_StatusTypeDef YS_SYN_Set(const uint8_t *info_data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif
