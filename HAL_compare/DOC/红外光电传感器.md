# HW

极简红外对管/避障开关读取：

- 文件：`BSP/HW/HW.h`、`BSP/HW/HW.c`
- 默认引脚：`PB13`
- 接口：`HW_Init()`、`HW_GetData()`

---

## 1. CubeMX 配置

- `PB13`：输入，上拉

---

## 2. 接线

| HW 模块 | STM32 |
|---|---|
| OUT | PB13 |
| VCC | 3.3V 或模块要求电压 |
| GND | GND |

---

## 3. 使用

```c
#include "HW.h"

HW_Init();
uint16_t blocked = HW_GetData();
```

第一次测试时你应该看到：

- 函数返回 `0/1`
- 该接口内部做了取反：引脚低电平时返回 `1`

---

## 4. 注意事项

- `HW_Init()` 当前为空实现，主要依赖 CubeMX 的 GPIO 初始化
