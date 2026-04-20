# Pot

极简电位器读取驱动：

- 文件：`BSP/Pot/pot.h`、`BSP/Pot/pot.c`
- 依赖：`BSP/AD/AD.h`
- 接口：`Pot_GetData()`、`Pot_GetVoltage()`

---

## 1. CubeMX 配置

- 先完成 AD 模块的 `ADC + DMA` 配置
- Pot 对应 ADC 通道引脚：待确认

---

## 2. 接线

| Pot 模块 | STM32 |
|---|---|
| AO | 待确认 |
| VCC | 3.3V |
| GND | GND |

---

## 3. 使用

```c
#include "AD.h"
#include "pot.h"

AD_init();
uint16_t raw = Pot_GetData();
float v = Pot_GetVoltage();
```

第一次测试时你应该看到：

- 旋钮变化时 `raw` 和 `v` 随之变化

---

## 4. 注意事项

- 当前实现读取 `AD_Value[1]`
- 若 AD 只开了 1 路 DMA（长度为1），需同步检查索引与通道配置
