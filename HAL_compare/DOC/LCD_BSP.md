# LCD

极简 TFT-LCD 驱动（FSMC 并口）：

- 文件：`BSP/LCD/lcd.h`、`BSP/LCD/lcd.c`
- 默认外设：`FSMC`（16bit 总线）
- 接口：`LCD_Init()`、`LCD_Clear()`、`LCD_ShowString()`

---

## 1. CubeMX 配置

- 需要开启 `FSMC`，并配置对应数据/控制引脚
- 工程中使用的是 `FSMC_NORSRAM_BANK4`
- `PB15` 在初始化时被置高（具体功能脚含义待确认）

---

## 2. 接线

该模块为并口屏，接线依赖具体屏幕排线定义，以下信息可确认：

- 通过 FSMC 总线访问
- 详细引脚请以 `F407ZGT6_HAL/Core/Src/fsmc.c` 实际配置为准

---

## 3. 使用

```c
#include "lcd.h"

LCD_Init();
LCD_Clear(WHITE);
LCD_ShowString(0, 0, 240, 24, 24, (u8*)"Hello");
```

第一次测试时你应该看到：

- 初始化后屏幕清屏（默认白底）
- 随后可显示字符串或字模

---

## 4. 注意事项

- `lcd.c` 引用了 `font.h`，仓库中实际文件名是 `FONT.H`
- `BSP/LCD/需要添加的函数.c` 与 `lcd.c` 存在同名函数，避免重复编译
- `PB15` 与部分其他模块定义有冲突风险，使用时注意引脚复用
