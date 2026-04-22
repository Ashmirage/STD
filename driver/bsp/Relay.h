#ifndef __RELAY_H
#define __RELAY_H

#include <stdint.h>

/* 初始化继电器控制引脚，默认上电后保持断开。 */
void Relay_init(void);

/* 吸合继电器，当前模块使用高电平有效。 */
void Relay_on(void);

/* 释放继电器。 */
void Relay_off(void);

/* 按状态控制继电器，0=断开，非 0=吸合。 */
void Relay_status(uint8_t status);

#endif
