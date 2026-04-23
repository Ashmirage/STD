- 可以直接使用

- 触摸功能需要包含他给的那些文件,也可以直接使用,使用方法如下
  ```c
  // 初始化需要这两行代码
  LCD_Init(); //LCD显示屏初始化
  TP_Init();
  
  // 使用的时候需要放到20ms执行一次的调度器里面
  // 20ms执行一次
  static void Loop_50hz(void)
  {
  	if (tp_dev.scan(0))  // 正常扫描，电容屏就这么用
      {
          uint16_t x = tp_dev.x[0];
          uint16_t y = tp_dev.y[0];
  		Send_printf("x=%d,y=%d\r\n",x,y);
          // x,y 就是当前第 1 个触点的屏幕坐标
      }
  }
  
  ```

- 触摸可以显示轨迹
  ```c
  if (tp_dev.scan(0))  // 正常扫描，电容屏就这么用
      {
          uint16_t x = tp_dev.x[0];
          uint16_t y = tp_dev.y[0];
  		TP_Draw_Big_Point(x,y,RED);
  //		Send_printf("x=%d,y=%d\r\n",x,y);
          // x,y 就是当前第 1 个触点的屏幕坐标
      }
  ```

## 如果要显示中文, 你现在需要加哪些内容

只需要两样:

1. 在业务文件里包含头文件lcd_font.h,lcd_hz24.h
2. 准备你自己的 24x24 汉字字模

也就是:

```c
#include "lcd.h"
#include "lcd_hz24.h"
#include "lcd_font.h"
```

以后要加新汉字时, 不需要再改底层函数, 只继续补字模数组就行。

## 字模文件

工程里已经加了一个示例字模头文件:

- [lcd_font.h](c:\Users\A\Mycode\STD_F407ZGT6\driver\bsp\lcd_font.h)

里面放了 4 个示例汉字:

- `HZ24_NI`
- `HZ24_HAO`
- `HZ24_SHI`
- `HZ24_JIE`

## 推荐取模参数

用 `PCtoLCD2002` 取模时建议这样配:

- 字体大小: `24x24`
- 取模方式: `阴码`
- 排列方式: `逐行式`
- 位序: `顺向`
- 输出格式: `C51格式`

因为当前函数按这个格式读点阵。

每个汉字固定是 `72` 字节:

```text
24 行 x 每行 3 字节 = 72 字节
```

## 最简单的使用示例

```c
#include "lcd.h"
#include "lcd_hz24.h"
#include "lcd_font.h"

POINT_COLOR = RED;
BACK_COLOR = WHITE;

LCD_ShowHZ24_Dot(0,  0, HZ24_NI,  0);
LCD_ShowHZ24_Dot(24, 0, HZ24_HAO, 0);
LCD_ShowHZ24_Dot(48, 0, HZ24_SHI, 0);
LCD_ShowHZ24_Dot(72, 0, HZ24_JIE, 0);
```

这样会显示 `你好世界`。

## 以后新增汉字怎么加

1. 用 `PCtoLCD2002` 生成目标汉字的 `24x24` 点阵
2. 按 `const u8 HZ24_XXX[72] = {...};` 的格式粘进 [lcd_font.h](c:\Users\A\Mycode\STD_F407ZGT6\driver\bsp\lcd_font.h) 或你自己的字模头文件
3. 在业务代码里继续调用 `LCD_ShowHZ24_Dot(x, y, HZ24_XXX, 0);`
