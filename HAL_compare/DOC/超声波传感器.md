# HC-SR04

极简超声波驱动：

- 文件：`BSP/HCSR04/HCSR04.h`
- 定时器：`TIM2`
- 默认引脚：`TRIG=PE0`，`ECHO=PE1`
- 接口：`HCSR04_Init()`、`HCSR04_ReadCm()`

---

## 1. CubeMX 配置

### TIM2

- Clock Source: `Internal Clock`
- Prescaler: `84 - 1`
- Counter Period: `4294967295`
- Counter Mode: `Up`

效果：`TIM2` 以 **1 MHz** 计数，也就是 **1 计数 = 1 us**。

### GPIO

- `PE0`：`Output Push Pull`，`No pull`，默认低电平
- `PE1`：`Input`，`No pull`

生成代码后应有：

- `Core/Inc/tim.h`
- `Core/Src/tim.c`

---

## 2. 接线

| HC-SR04 | STM32 |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | PE0 |
| ECHO | PE1 |

> 注意：很多 HC-SR04 的 `ECHO` 是 **5V**，不确定时请做分压或电平转换。

---

## 3. 使用

```c
#include "OLED.h"
#include "HCSR04.h"

float distance_cm;              // 超声波测出来的距离，单位：cm

OLED_Init();
OLED_Clear();
HCSR04_Init();

while (1) {
    if (HCSR04_ReadCm(&distance_cm) == HAL_OK) {
        OLED_ShowString(1, 1, "DIST:");
        OLED_ShowNum(1, 6, (uint32_t)distance_cm, 3);
        OLED_ShowString(1, 9, "cm");
    } else {
        OLED_ShowString(1, 1, "HCSR04 FAIL ");
    }

    HAL_Delay(60);
}
```

第一次测试时你应该看到：

- 正常时，第1行显示距离，例如 `DIST:123cm`
- 挡住或移开前方物体时，数值会变化
- 如果一直显示 `HCSR04 FAIL`，优先检查 `TRIG / ECHO / 电源`

---

## 4. 改引脚

如果你换了引脚，只改 `HCSR04.h` 顶部宏：

```c
#define HCSR04_TRIG_GPIO_Port GPIOE
#define HCSR04_TRIG_Pin GPIO_PIN_0
#define HCSR04_ECHO_GPIO_Port GPIOE
#define HCSR04_ECHO_Pin GPIO_PIN_1
```
