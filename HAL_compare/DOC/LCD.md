## CubeMX配置
- 设置背光引脚PB15,输出模式, 默认输出高电平
- 在connectivity中选择FSMC配置
    - 选择LCD4
    - chip select : NE4
    - memory type:SRAM
    - ADDRESS:16bits
    - DATA: 16bits
- 下面的Configuration
    - Write operation:ENABLE
    - Extended mode: ENABLE
    - Address setuptime in HCLK :1
    - Data setuptime in HCLK:15
    - Bus turn around: 0
    - Extended address setup time:0
    - Extended data setuptime:3
    - Extended bus turn :0
## 代码修改
- lcd.h修改宏的配置:
`#define LCD_BASE        ((u32)(0x6C000000 | 0x0000007E))`
- lcd.c:
    - 初始化里面拉高背光引脚
    - 读一下IC驱动的ID: `lcddev.id = LCD_ReadReg(0x000);`
    - 下面一行的串口打印函数删除
    - 延时函数都replace成`HAL_Delay`

- 汉字显示
    - 取模软件配置:字体大小24*24  选择阴码  逐行式  顺向 十六进制数  段前段后缀没有   数据前缀:0x  数据后缀:,  行后缀: ,
- 代码修改:
    - 添加lcd_font.h文件
    - lcd.h文件,添加声明`void LCD_ShowHZ24_Dot(u16 x,u16 y,const u8 *dot,u8 mode);`
    - lcd.c文件,添加函数`void LCD_ShowHZ24_Dot(u16 x,u16 y,const u8 *dot,u8 mode);`


