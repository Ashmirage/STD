#ifndef __POT_H
#define __POT_H

#include <stdint.h>

/* 兼容旧接口，只保留电位器原始值读取。 */
void Pot_Init(void);

uint16_t Pot_GetData(void);

#endif
