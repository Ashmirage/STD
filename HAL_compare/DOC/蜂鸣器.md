# Buzzer

极简蜂鸣器驱动：

- 文件：`BSP/Buzzer/Buzzer.h`、`BSP/Buzzer/Buzzer.c`
- 默认引脚：`PF8`
- 接口：`Buzzer_init()`、`Buzzer_ON()`、`Buzzer_OFF()`、`Buzzer_alarm_start()`、`Buzzer_alarm(ms)`

---

## 1. CubeMX 配置

- `PF8`：`Output Push Pull`，`No pull`

---

## 2. 接线

| Buzzer | STM32 |
|---|---|
| 信号脚 | PF8 |
| VCC | 3.3V 或模块要求电压 |
| GND | GND |

---

## 3. 使用

```c
#include "Buzzer.h"

Buzzer_init();
Buzzer_alarm_start();
// 放到固定周期任务里反复调用，例如 20ms
Buzzer_alarm(20);
```

第一次测试时你应该看到：

- `Buzzer_ON()` 时蜂鸣器响
- `Buzzer_OFF()` 时蜂鸣器停
- 调用报警接口后可听到分段报警节奏

---

## 4. 注意事项

- 本模块是 GPIO 开关控制，更适合有源蜂鸣器
- 无源蜂鸣器如需不同音调，建议改为 PWM 驱动
