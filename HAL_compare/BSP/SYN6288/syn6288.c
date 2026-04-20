#include "SYN6288.h"

#include <stddef.h>

#define SYN6288_TEXT_MODE_UNICODE  0x03U

static UART_HandleTypeDef *syn6288_uart = NULL;

const uint8_t SYN_StopCom[SYN6288_CONTROL_FRAME_LEN] = {0xFD, 0x00, 0x02, 0x02,
                                                        0xFD};
const uint8_t SYN_SuspendCom[SYN6288_CONTROL_FRAME_LEN] = {
    0xFD, 0x00, 0x02, 0x03, 0xFC};
const uint8_t SYN_RecoverCom[SYN6288_CONTROL_FRAME_LEN] = {0xFD, 0x00, 0x02, 0x04,
                                                           0xFB};
const uint8_t SYN_ChackCom[SYN6288_CONTROL_FRAME_LEN] = {0xFD, 0x00, 0x02, 0x21,
                                                         0xDE};
const uint8_t SYN_PowerDownCom[SYN6288_CONTROL_FRAME_LEN] = {
    0xFD, 0x00, 0x02, 0x88, 0x77};

void SYN6288_Init(UART_HandleTypeDef *huart) { syn6288_uart = huart; }

static HAL_StatusTypeDef SYN6288_SendUnicodeFrame(uint8_t music,
                                                  const uint8_t *data,
                                                  uint16_t length) {
  uint8_t frame_info[SYN6288_MAX_TEXT_BYTES + 6U];
  uint8_t ecc = 0U;
  uint16_t frame_length = 0U;
  uint16_t data_length = 0U;
  uint16_t i = 0U;

  if ((syn6288_uart == NULL) || (data == NULL) || (length == 0U)) {
    return HAL_ERROR;
  }

  if (length > SYN6288_MAX_TEXT_BYTES) {
    return HAL_ERROR;
  }

  data_length = length + 3U;
  frame_info[0] = 0xFD;
  frame_info[1] = (uint8_t)(data_length >> 8);
  frame_info[2] = (uint8_t)data_length;
  frame_info[3] = 0x01;
  frame_info[4] = (uint8_t)(SYN6288_TEXT_MODE_UNICODE | ((music & 0x0FU) << 4));

  for (i = 0U; i < 5U; i++) {
    ecc ^= frame_info[i];
  }

  for (i = 0U; i < length; i++) {
    frame_info[5U + i] = data[i];
    ecc ^= frame_info[5U + i];
  }

  frame_info[5U + length] = ecc;
  frame_length = length + 6U;

  return HAL_UART_Transmit(syn6288_uart, frame_info, frame_length,
                           SYN6288_TX_TIMEOUT_MS);
}

static HAL_StatusTypeDef SYN6288_Utf8ToUnicode(const char *utf8_text,
                                               uint8_t *unicode_data,
                                               uint16_t *unicode_length) {
  const uint8_t *src = (const uint8_t *)utf8_text;
  uint16_t out_len = 0U;

  if ((utf8_text == NULL) || (unicode_data == NULL) || (unicode_length == NULL)) {
    return HAL_ERROR;
  }

  while (*src != '\0') {
    uint16_t code_unit = 0U;

    if ((*src & 0x80U) == 0U) {
      code_unit = *src++;
    } else if (((src[0] & 0xE0U) == 0xC0U) &&
               ((src[1] & 0xC0U) == 0x80U)) {
      code_unit = (uint16_t)(((src[0] & 0x1FU) << 6) | (src[1] & 0x3FU));
      if (code_unit < 0x80U) {
        return HAL_ERROR;
      }
      src += 2;
    } else if (((src[0] & 0xF0U) == 0xE0U) &&
               ((src[1] & 0xC0U) == 0x80U) &&
               ((src[2] & 0xC0U) == 0x80U)) {
      code_unit = (uint16_t)(((src[0] & 0x0FU) << 12) |
                             ((src[1] & 0x3FU) << 6) |
                             (src[2] & 0x3FU));
      if ((code_unit < 0x800U) ||
          ((code_unit >= 0xD800U) && (code_unit <= 0xDFFFU))) {
        return HAL_ERROR;
      }
      src += 3;
    } else {
      return HAL_ERROR;
    }

    if ((out_len + 2U) > SYN6288_MAX_TEXT_BYTES) {
      return HAL_ERROR;
    }

    unicode_data[out_len++] = (uint8_t)(code_unit >> 8);
    unicode_data[out_len++] = (uint8_t)code_unit;
  }

  *unicode_length = out_len;
  return HAL_OK;
}

HAL_StatusTypeDef SYN_FrameInfoUtf8(uint8_t music, const char *utf8_text) {
  uint8_t unicode_data[SYN6288_MAX_TEXT_BYTES];
  uint16_t unicode_length = 0U;

  if (SYN6288_Utf8ToUnicode(utf8_text, unicode_data, &unicode_length) != HAL_OK) {
    return HAL_ERROR;
  }

  return SYN6288_SendUnicodeFrame(music, unicode_data, unicode_length);
}

HAL_StatusTypeDef YS_SYN_Set(const uint8_t *info_data, uint16_t length) {
  if ((syn6288_uart == NULL) || (info_data == NULL) || (length == 0U)) {
    return HAL_ERROR;
  }

  return HAL_UART_Transmit(syn6288_uart, (uint8_t *)info_data, length,
                           SYN6288_TX_TIMEOUT_MS);
}
