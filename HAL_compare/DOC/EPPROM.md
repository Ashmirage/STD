## Cubemx配置
PB8 配置为推挽输出,作为IIC时钟线
PB9 配置为开漏输出,作为IIC数据线
## 代码
需要初始化, 然后进行check函数检查模块是否正常,然后指定地址写,指定地址读就行了.
这个check函数正常时返回0,不正常返回1,需要注意
需要软件IIC,包含my_iic文件
```
int main(void)
{
    uint8_t key;
    uint16_t i = 0;
    uint8_t datatemp[TEXT_SIZE];

    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7);         /* 设置时钟,168Mhz */
    delay_init(168);                            /* 延时初始化 */
    usart_init(115200);                         /* 串口初始化为115200 */
    usmart_dev.init(84);                        /* 初始化USMART */
    led_init();                                 /* 初始化LED */
    lcd_init();                                 /* 初始化LCD */
    key_init();                                 /* 初始化按键 */
    at24cxx_init();                             /* 初始化24CXX */

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "IIC TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY1:Write  KEY0:Read", RED);    /* 显示提示信息 */

    while (at24cxx_check()) /* 检测不到24c02 */
    {
        lcd_show_string(30, 130, 200, 16, 16, "24C02 Check Failed!", RED);
        delay_ms(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check!      ", RED);
        delay_ms(500);
        LED0_TOGGLE();      /* 红灯闪烁 */
    }

    lcd_show_string(30, 130, 200, 16, 16, "24C02 Ready!", RED);

    while (1)
    {
        key = key_scan(0);

        if (key == KEY1_PRES)   /* KEY1按下,写入24C02 */
        {
            lcd_fill(0, 150, 239, 319, WHITE);  /* 清除半屏 */
            lcd_show_string(30, 150, 200, 16, 16, "Start Write 24C02....", BLUE);
            at24cxx_write(0, (uint8_t *)g_text_buf, TEXT_SIZE);
            lcd_show_string(30, 150, 200, 16, 16, "24C02 Write Finished!", BLUE);   /* 提示传送完成 */
        }

        if (key == KEY0_PRES)   /* KEY0按下,读取字符串并显示 */
        {
            lcd_show_string(30, 150, 200, 16, 16, "Start Read 24C02.... ", BLUE);
            at24cxx_read(0, datatemp, TEXT_SIZE);
            lcd_show_string(30, 150, 200, 16, 16, "The Data Readed Is:  ", BLUE);   /* 提示传送完成 */
            lcd_show_string(30, 170, 200, 16, 16, (char *)datatemp, BLUE);          /* 显示读到的字符串 */
        }

        i++;

        if (i == 20)
        {
            LED0_TOGGLE();  /* 红灯闪烁 */
            i = 0;
        }

        delay_ms(10);
    }
}

```