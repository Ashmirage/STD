# Motor

极简直流电机驱动（方向 + PWM）：

- 文件：`BSP/Motor/Motor.h`、`BSP/Motor/Motor.c`
- 默认引脚：`AIN1=PA4`，`AIN2=PA6`
- 默认 PWM：`TIM3 CH4`
- 接口：`Motor_init()`、`Motor_set_speed()`、`Motor_get_status()`

---

## 1. CubeMX 配置

- `PA4`、`PA6`：输出推挽（方向控制）
- `TIM3 CH4`：PWM 输出（占空比控制速度）

---

## 2. 接线

| 电机驱动板 | STM32 |
|---|---|
| AIN1 | PA4 |
| AIN2 | PA6 |
| PWM | TIM3_CH4 对应引脚 |
| GND | GND |
STBY->3.3V
VM->5V
VCC->3.3

---

## 3. 使用

```c
#include "Motor.h"

Motor_init();
Motor_set_speed(60);   // 正转
Motor_set_speed(0);    // 停机
```

第一次测试时你应该看到：

- `speed > 0` 与 `speed < 0` 方向相反
- `speed = 0` 时输出关闭并停机

---

## 4. 注意事项

- 速度输入会被限幅到 `-100~100`
- 代码依赖 `htim3`，请确认你的工程已启用 `TIM3`
- 不建议高频大幅正反切换
