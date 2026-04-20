# Stepmotor

极简步进电机驱动（非阻塞节拍）：

- 文件：`BSP/Stepmotor/stepmotor.h`、`BSP/Stepmotor/stepmotor.c`
- 默认引脚：`PA0`、`PA4`、`PA5`、`PA6`
- 接口：`STEPMOTOR_Init()`、`Stepmotor_angle_dir()`、`Stepmotor_Rhythm_1ms()`

---

## 1. CubeMX 配置

- 四相输出引脚配置为 GPIO 输出
- 需要 1ms 周期调度以调用 `Stepmotor_Rhythm_1ms()`

---

## 2. 接线

| 步进电机驱动板 | STM32 |
|---|---|
| IN1 | PA0 |
| IN2 | PA4 |
| IN3 | PA5 |
| IN4 | PA6 |
| GND | GND |

---

## 3. 使用

```c
#include "stepmotor.h"

STEPMOTOR_Init();
Stepmotor_angle_dir(0, 90, 1);
Stepmotor_Rhythm_1ms();   // 放在1ms任务中反复调用
```

第一次测试时你应该看到：

- 下发角度命令后电机按设定方向转动
- 达到目标步数后自动停止

---

## 4. 注意事项

- 该驱动不是阻塞延时转动，必须持续喂 `Stepmotor_Rhythm_1ms()`
- 运行中再次下发命令会被忽略
- 当前 `STEPMOTOR_Init()` 内 GPIO 初始化代码是注释状态，需确认外部已正确初始化
