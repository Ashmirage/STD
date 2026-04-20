# Matrix Keyboard

极简 4x4 矩阵键盘驱动：

- 文件：`BSP/Matrix_keyboard/Matrix_keyboard.h`
- 实现：`BSP/Matrix_keyboard/Matrix_keyboard.c`
- 接口：`Matrix_keyboard_init()`、`Matrix_keyboard_get_number()`

---

## 1. 当前引脚映射

### 列线

- `C1 -> PF0`
- `C2 -> PF2`
- `C3 -> PF4`
- `C4 -> PF6`

### 行线

- `R1 -> PE6`
- `R2 -> PE4`
- `R3 -> PE2`
- `R4 -> PE0`

---

## 2. CubeMX 怎么配

这个驱动的扫描方式是：

- 行线由 STM32 主动输出
- 列线由 STM32 读取输入电平
- 扫描时一次只拉高一行，去读四列

所以 CubeMX 这样配：

### 行线 R1~R4

- `PE6`：`GPIO_Output`
- `PE4`：`GPIO_Output`
- `PE2`：`GPIO_Output`
- `PE0`：`GPIO_Output`

建议参数：

- Mode: `Output Push Pull`
- Pull-up/Pull-down: `No pull`
- Speed: `Low`
- Initial Level: `Low`

### 列线 C1~C4

- `PF0`：`GPIO_Input`
- `PF2`：`GPIO_Input`
- `PF4`：`GPIO_Input`
- `PF6`：`GPIO_Input`

建议参数：

- Pull-up/Pull-down: `Pull-down`

这样当某一行被拉高且某列被按下时，对应列输入就会读到高电平。

---

## 3. 键值对应关系

这个驱动返回的键值是：

```text
R1:  1   2   3   4
R2:  5   6   7   8
R3:  9  10  11  12
R4: 13  14  15  16
```

也就是：

- `R1` 这一行按键：`1 2 3 4`
- `R2` 这一行按键：`5 6 7 8`
- `R3` 这一行按键：`9 10 11 12`
- `R4` 这一行按键：`13 14 15 16`

> 说明：这里按你已经验证过的**物理按键显示效果**来描述。
> 也就是说，最终以你实际按键时 OLED 显示出来的数字对应关系为准。

---

## 4. 使用

```c
#include "OLED.h"
#include "Matrix_keyboard.h"

uint8_t key_num;

OLED_Init();
OLED_Clear();
Matrix_keyboard_init();

OLED_ShowString(1, 1, "KEY:");
OLED_ShowString(1, 5, "0 ");

while (1) {
    key_num = Matrix_keyboard_get_number();
    if (key_num == 0) {
        OLED_ShowString(1, 5, "0 ");
    } else if (key_num < 10) {
        OLED_ShowNum(1, 5, key_num, 1);
        OLED_ShowString(1, 6, " ");
    } else {
        OLED_ShowNum(1, 5, key_num, 2);
    }
    HAL_Delay(20);
}
```

---

## 5. 说明

- `Matrix_keyboard_get_number()` 带了简单消抖
- 返回值范围是 `1~16`
- 没按键时返回 `0`
- 当前逻辑是“默认返回 0，按下稳定后持续返回当前键值，松手后回到 0”

如果空载时会在 `0` 和某个键值之间跳动，优先检查：

- 对应列输入脚有没有配置成 `Pull-down`
- 杜邦线或排线是否接触不稳
- 对应按键是否有物理卡键或漏导通

如果你只想显示数字键本身，那么：

- `1~9` 可以直接显示
- `10~16` 也是会正常返回的，只是 OLED 上会显示成两位数
