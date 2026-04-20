# TM1637

极简四位数码管驱动：

- 文件：`BSP/TM1637/TM1637.h`
- 默认引脚：`CLK=PF14`，`DIO=PF12`
- 接口：`TM1637_Init()`、`TM1637_Display()`

---

## 1. CubeMX 配置

- `PF14`：`Output Push Pull`，`No pull`
- `PF12`：`Output Push Pull`，`No pull`

---

## 2. 接线

| TM1637 | STM32 |
|---|---|
| VCC | 3.3V |
| GND | GND |
| CLK | PF14 |
| DIO | PF12 |

---

## 3. 使用

```c
#include "OLED.h"
#include "TM1637.h"

uint8_t digits[4] = {           // 四位数码管要显示的4个数字
    TM1637_NUM[1],
    TM1637_NUM[2],
    TM1637_NUM[3],
    TM1637_NUM[4]
};
TM1637_Init();
TM1637_Display(digits);
```

第一次测试时你应该看到：

- TM1637 数码管显示 `1234`
- 如果数码管不亮，优先检查 `VCC / GND / CLK / DIO`

---

## 4. 改引脚

如果你换了引脚，只改 `TM1637.h` 顶部宏：

```c
#define TM1637_CLK(x) HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, x)
#define TM1637_DIO(x) HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, x)
```
