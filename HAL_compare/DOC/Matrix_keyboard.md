# Matrix_keyboard

极简 4x4 矩阵键盘驱动：

- 文件：`BSP/Matrix_keyboard/Matrix_keyboard.h`、`BSP/Matrix_keyboard/Matrix_keyboard.c`
- 默认接口：4 行 + 4 列 GPIO 扫描
- 接口：`Matrix_keyboard_init()`、`Matrix_keyboard_get_number()`

---

## 1. CubeMX 配置

- 行引脚：输出推挽（默认拉低）
- 列引脚：输入模式
- 驱动内部带 10ms 消抖（`HAL_GetTick()`）

---

## 2. 接线

| 矩阵键盘 | STM32 |
|---|---|
| Row1 | PE6 |
| Row2 | PE4 |
| Row3 | PE2 |
| Row4 | PE0 |
| Col1 | PC0 |
| Col2 | PC1 |
| Col3 | PC2 |
| Col4 | PC3 |

---

## 3. 使用

```c
#include "Matrix_keyboard.h"

Matrix_keyboard_init();
uint8_t key = Matrix_keyboard_get_number();
if (key != 0) {
    // key: 1~16
}
```

第一次测试时你应该看到：

- 按下并松开后返回一个 `1~16` 键值
- 未按键时返回 `0`

---

## 4. 注意事项

- 当前工程里键盘初始化调用是注释状态，需在你的业务初始化中启用
- 与 `LCD`/`Motor` 存在引脚复用冲突风险（如 `PB15`、`PA4`、`PA6`）
