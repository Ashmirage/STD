# UI 模块代码文档

本文档根据当前 `driver/bsp/UI.c` 和 `driver/bsp/UI.h` 生成，按功能模块整理，并把对应源码原封不动列出，方便比赛时查找和抄写。

## 1. 头文件接口

头文件声明 UI 对外提供的初始化、状态机、数据更新、显示刷新接口，以及业务状态枚举。

```c
#ifndef __UI_H
#define __UI_H		
#include "stm32f4xx.h"

void APP_init(void); //UI初始化

void APP(u16 ms); //APP状态机

// 更新数据,20ms执行一次
void APP_data_update(void);

// 显示信息,2s执行一次
void APP_show_info(void);


// 业务状态枚举
typedef enum{
	APP_CHANGE_PASSWORD, //修改密码状态
	APP_LOCKED, //未解锁状态
	APP_PRE_UNLOCKED, //输入密码的状态
	APP_UNLOCKED, //解锁状态
}APP_STATUS;

extern volatile APP_STATUS app_status;// 定义业务状态
#endif
```

## 2. 依赖和外部函数

这一部分包含 UI.c 使用到的头文件和外部 IO / LED 翻转函数声明。

```c
#include "UI.h"        // 业务
#include "lcd.h"       //显示屏
#include "delay.h"      //延时
#include "W25qxx.h"  //存储
#include "Matrix_keyboard.h" //矩阵键盘
#include "lcd_hz24.h"
#include <string.h>

extern void io0(void);//三个IO初始化
extern void gt(void); // 绿色LED翻转
extern void rt(void); //红色LED翻转
extern void bt(void); //蜂鸣器翻转
```

## 3. 滚动动画辅助函数

这一部分负责计算字符串宽度、按坐标显示移动字符串，以及按剩余时间延时。

```c
#define UI_FONT_SIZE   24
#define UI_CHAR_WIDTH  (UI_FONT_SIZE / 2)

// 计算一行 ASCII 字符串显示出来有多宽
static uint16_t UI_StringWidth(char *str)
{
	uint16_t len = 0;

	while (str[len] != '\0')len++;
	return len * UI_CHAR_WIDTH;
}

// 在指定坐标显示一行字符串，超出屏幕的字符不画
static void UI_ShowMoveString(char *str, int16_t x, int16_t y)
{
	uint16_t i = 0;
	int16_t show_x;

	if (y < 0 || y > (int16_t)lcddev.height - UI_FONT_SIZE)return;

	while (str[i] != '\0')
	{
		show_x = x + (int16_t)(i * UI_CHAR_WIDTH);
		if (show_x >= 0 && show_x < (int16_t)lcddev.width)
		{
			LCD_ShowChar((u16)show_x, (u16)y, (u8)str[i], UI_FONT_SIZE, 1);
		}
		i++;
	}
}

// 延时不超过剩余滚动时间，时间到了就尽快结束
static void UI_DelayInTime(uint32_t start, uint32_t time_ms, uint16_t wait_ms)
{
	uint32_t elapsed = SysTick_GetTick() - start;
	uint32_t remain;

	if (elapsed >= time_ms)return;

	remain = time_ms - elapsed;
	if (remain > wait_ms)remain = wait_ms;
	delay_ms((uint16_t)remain);
}
```

## 4. 滚动动画函数

这一部分是四个方向的滚动动画函数，参数 `time_ms` 表示滚动持续时间，时间到后清黑屏。

```c
// 从左向右滚动：time_ms 是滚动时间，单位 ms
void Lcd_scroll_left_to_right(uint32_t time_ms)
{
	char row1[] = "ZNJJXT"; // 第一行内容，比赛时直接改这里
	char row2[] = "0208";   // 第二行内容，比赛时直接改这里
	uint16_t speed = 4;      // 每次移动的像素，越大越快
	uint16_t wait_ms = 30;   // 每次移动后的延时，越大越慢
	uint16_t w1 = UI_StringWidth(row1);
	uint16_t w2 = UI_StringWidth(row2);
	uint16_t block_w = w1 > w2 ? w1 : w2;
	int16_t row1_y = (int16_t)(lcddev.height / 4U);
	int16_t row2_y = (int16_t)(lcddev.height / 2U);
	int16_t x = -(int16_t)block_w; // 起点在屏幕左侧外面
	uint32_t start = SysTick_GetTick();

	while (SysTick_GetTick() - start < time_ms)
	{
		LCD_Clear(WHITE);
		UI_ShowMoveString(row1, x + (int16_t)((block_w - w1) / 2U), row1_y);
		UI_ShowMoveString(row2, x + (int16_t)((block_w - w2) / 2U), row2_y);
		UI_DelayInTime(start, time_ms, wait_ms);

		x += (int16_t)speed;
		if (x > (int16_t)lcddev.width)x = -(int16_t)block_w;
	}

	LCD_Clear(BLACK); // 时间到，直接熄屏
}

// 从右向左滚动：time_ms 是滚动时间，单位 ms
void Lcd_scroll_right_to_left(uint32_t time_ms)
{
	char row1[] = "ZNJJXT"; // 第一行内容，比赛时直接改这里
	char row2[] = "0208";   // 第二行内容，比赛时直接改这里
	uint16_t speed = 4;      // 每次移动的像素，越大越快
	uint16_t wait_ms = 30;   // 每次移动后的延时，越大越慢
	uint16_t w1 = UI_StringWidth(row1);
	uint16_t w2 = UI_StringWidth(row2);
	uint16_t block_w = w1 > w2 ? w1 : w2;
	int16_t row1_y = (int16_t)(lcddev.height / 4U);
	int16_t row2_y = (int16_t)(lcddev.height / 2U);
	int16_t x = (int16_t)lcddev.width; // 起点在屏幕右侧外面
	uint32_t start = SysTick_GetTick();

	while (SysTick_GetTick() - start < time_ms)
	{
		LCD_Clear(WHITE);
		UI_ShowMoveString(row1, x + (int16_t)((block_w - w1) / 2U), row1_y);
		UI_ShowMoveString(row2, x + (int16_t)((block_w - w2) / 2U), row2_y);
		UI_DelayInTime(start, time_ms, wait_ms);

		x -= (int16_t)speed;
		if (x < -(int16_t)block_w)x = (int16_t)lcddev.width;
	}

	LCD_Clear(BLACK); // 时间到，直接熄屏
}

// 从上向下滚动：time_ms 是滚动时间，单位 ms
void Lcd_scroll_top_to_bottom(uint32_t time_ms)
{
	char row1[] = "ZNJJXT"; // 第一行内容，比赛时直接改这里
	char row2[] = "0208";   // 第二行内容，比赛时直接改这里
	uint16_t speed = 4;      // 每次移动的像素，越大越快
	uint16_t wait_ms = 30;   // 每次移动后的延时，越大越慢
	uint16_t w1 = UI_StringWidth(row1);
	uint16_t w2 = UI_StringWidth(row2);
	int16_t row_gap = (int16_t)(lcddev.height / 4U);
	int16_t row1_x = ((int16_t)lcddev.width - (int16_t)w1) / 2;
	int16_t row2_x = ((int16_t)lcddev.width - (int16_t)w2) / 2;
	int16_t block_h = row_gap + UI_FONT_SIZE;
	int16_t y = -block_h; // 起点在屏幕上方外面
	uint32_t start = SysTick_GetTick();

	while (SysTick_GetTick() - start < time_ms)
	{
		LCD_Clear(WHITE);
		UI_ShowMoveString(row1, row1_x, y);
		UI_ShowMoveString(row2, row2_x, y + row_gap);
		UI_DelayInTime(start, time_ms, wait_ms);

		y += (int16_t)speed;
		if (y > (int16_t)lcddev.height)y = -block_h;
	}

	LCD_Clear(BLACK); // 时间到，直接熄屏
}

// 从下向上滚动：time_ms 是滚动时间，单位 ms
void Lcd_scroll_bottom_to_top(uint32_t time_ms)
{
	char row1[] = "ZNJJXT"; // 第一行内容，比赛时直接改这里
	char row2[] = "0208";   // 第二行内容，比赛时直接改这里
	uint16_t speed = 4;      // 每次移动的像素，越大越快
	uint16_t wait_ms = 30;   // 每次移动后的延时，越大越慢
	uint16_t w1 = UI_StringWidth(row1);
	uint16_t w2 = UI_StringWidth(row2);
	int16_t row_gap = (int16_t)(lcddev.height / 4U);
	int16_t row1_x = ((int16_t)lcddev.width - (int16_t)w1) / 2;
	int16_t row2_x = ((int16_t)lcddev.width - (int16_t)w2) / 2;
	int16_t block_h = row_gap + UI_FONT_SIZE;
	int16_t y = (int16_t)lcddev.height; // 起点在屏幕下方外面
	uint32_t start = SysTick_GetTick();

	while (SysTick_GetTick() - start < time_ms)
	{
		LCD_Clear(WHITE);
		UI_ShowMoveString(row1, row1_x, y);
		UI_ShowMoveString(row2, row2_x, y + row_gap);
		UI_DelayInTime(start, time_ms, wait_ms);

		y -= (int16_t)speed;
		if (y < -block_h)y = (int16_t)lcddev.height;
	}

	LCD_Clear(BLACK); // 时间到，直接熄屏
}
```

## 5. 全局状态和初始化

这一部分定义业务状态、密码缓存、清空字符串，并在上电初始化时播放滚动动画和读取 Flash 密码。

```c
volatile APP_STATUS app_status;// 定义业务状态
char blank[] = "                          "; // 清空字符

char default_password[7] = "123456"; //默认密码 ,写入密码之后改成变成空串

//上电后的业务初始化代码
void APP_init(void)
{
	Lcd_scroll_bottom_to_top(3000); //开机滚动状态,参数:持续时间
	app_status = APP_LOCKED; //设置初始状态
	//从flash中读取密码
	W25QXX_Read(default_password,0,7) ; // 最先开始应该先写一次
	// 滚动结束后熄屏
	LCD_Clear(BLACK);
}
```

## 6. 密码输入和显示

这一部分处理键盘输入密码、密码校验、错误次数，以及屏幕上星号显示。

```c
typedef struct
{
	uint8_t word_count;
	uint8_t is_true;
}password_st;


u32 error_t; // 错误次数


// 输入密码的函数,被APP_show_password_text()调用
password_st Input_password(u16 ms)
{
	static char input[7]; //缓存输入的密码
	static uint8_t error_count = 0; //错误次数
	static uint8_t count = 0;  //输入字符数
	static uint8_t status = 0;    //状态
	uint8_t num = MK_Get(); // 读取一个数字
	static password_st password_structure;  //初始化结构体
	password_structure.is_true = 0; //默认失败
	switch(status)
	{
		case 0:
			if(num != 0 && num < 10)// 如果键盘有输入而且是0-9的数字
			{
				input[count] = num + '0';//存储字符串
				count++;
				if(count == 6) //已经输入六个数
				{
					input[count] = '\0'; //结束标识符
					if(strcmp(input,default_password) == 0) // 比较密码,等于0表示密码正确
					{
						error_count = 0; //清除错误次数
						/*
						解锁操作写在这里
						*/
						status = 2;   // 进入下一个状态
					}else //密码错误
					{
						status = 1;
						LCD_Clear(WHITE); //清屏
					}
				}
			}
		break;
		case 1: //密码错误
			if(error_count >= 2)
			{
				/*
				报警提示写在这里
				*/
				error_count = 0;
				count = 0;//重置状态
				status = 0;
				LCD_Clear(WHITE); //清屏
			}else
			{
				error_count++;//错误次数++
				count = 0;//重置状态
				status = 0;
				LCD_Clear(WHITE); //清屏
				/*
				再次显示输入密码
				*/
//					LCD_ShowHZ24_Dot(0,200,shu,0);
//					LCD_ShowHZ24_Dot(24,200,ru,0);
//					LCD_ShowHZ24_Dot(48,200,mi,0);
//					LCD_ShowHZ24_Dot(72,200,ma,0);
			}
		break;
		case 2://密码正确
			/*
			密码正确的操作写在这里
			*/	
//			LED_flash_off();//5s自动关闭
//			Relay_on(); //打开继电器
			status = 0;
			password_structure.is_true = 1;
			gt();
			count = 0;
			break;
	}
	password_structure.word_count = count; //已经输入的长度
	return password_structure;
}

// 显示输入密码******的函数,被状态机调用
uint8_t APP_show_password_text(u16 ms)
{ 
	static uint8_t last_len = 0;
	uint16_t i;
	password_st temp = Input_password(ms);
	uint8_t len = temp.word_count; //获取已经输入的密码长度
	if(last_len != len)
	{
		LCD_ShowString(0,250,320,24,24,blank); //清空区域
	}
	for(i = 0;i < len;i++)
	{
		LCD_ShowChar(i * 30,250,'*',24,1); //显示*
	}
	last_len = len;
	if(temp.is_true == 1){
		 last_len = len = 0;
	}
	return temp.is_true; // 返回是否密码正确
}
```

## 7. 修改密码

这一部分处理新密码输入、写入 Flash，以及修改密码时的星号显示。

```c
password_st APP_change_password(void)
{
	static char input[7]; //缓存输入的密码
	static uint8_t count = 0;
	uint8_t num = MK_Get();
	uint16_t i;
	static password_st password_structure;
	password_structure.is_true = 0; //默认失败
	if(num != 0 && num < 10)// 如果键盘有输入而且是0-9的数字
	{
		input[count] = num + '0';//存储字符串
		count++;
		if(count == 6) //是否已经输入六个数
		{
			input[count] = '\0'; //结束标识符
			password_structure.is_true = 1;
			for(i = 0;i < 7;i++)
			{
				default_password[i] = input[i]; // 修改原密码
			}
			W25QXX_Write(default_password,0,7); //写入flash,掉电不丢失
			count = 0; //清零count,以便下次修改
		}
	}
	password_structure.word_count = count; //已经输入的长度
	return password_structure;
}

// 显示修改密码的函数,被状态机调用
uint8_t APP_change_password_text(u16 ms)
{ 
	static uint8_t last_len = 0;
	uint16_t i;
	password_st temp = APP_change_password();
	uint8_t len = temp.word_count; //获取已经输入的密码长度
	if(last_len != len)
	{
		LCD_ShowString(0,250,320,24,24,blank);
	}
	for(i = 0;i < len;i++)
	{
		LCD_ShowChar(i * 30,250,'*',24,1);
	}
	last_len = len;
	return temp.is_true;
}
```

## 8. 数据更新

这一部分预留传感器和执行器状态的数据更新位置，当前只有解锁状态才会更新。

```c
//这里定义显示的数据量
//*/
////static uint16_t temp = 0;  //温度
////static uint16_t hum = 0;      // 湿度
////static uint16_t light = 0;           //光照
////static uint8_t motor_status = 0;           //直流电机状态
////static uint8_t led_status = 0;                           // 灯状态

///*
//如果有控制模式,定义控制模式
//*/
//////定义控制模式,默认手动
////static uint8_t control_mode = 0;

// 更新数据,20ms执行一次
void APP_data_update(void)
{
	//处于解锁状态才更新数据
	if(app_status == APP_UNLOCKED)
	{
		/*
		这里写需要获取的所有数据
		*/
//		temp = (uint16_t)AD_GetTempCelsius(); //更新温度
//		hum = dht11_h; //更新湿度
//		light = AD_GetLdrLux(); //更新光照
//		motor_status = Motor_get_status(); //更新直流电机状态
```

## 9. 手动控制

这一部分在解锁状态下读取按键命令，目前按键 6 进入修改密码状态。

```c
	}
}

// 控制的函数
void APP_control(void)
{
	//判断是否处于解锁状态,并且手动控制
	if(app_status == APP_UNLOCKED)
	{
		uint8_t command = MK_Get(); //获取键盘输入值
		/*
		这个地方比赛的时候根据题目发挥,写各种按键控制
		*/
		if(command == 6) // 修改密码
		{
			app_status = APP_CHANGE_PASSWORD; //切换状态
			LCD_Clear(WHITE); //清屏
			LCD_ShowString(0,200,320,24,24,"please input password:");
			return;
		}

	}
}
```

## 10. 信息显示

这一部分用于周期性刷新屏幕显示内容。

```c

// 屏幕刷新信息,2s执行一次
void APP_show_info(void)
{
	LCD_ShowString(0,300,300,24,24,"I am a instance");
	/**/
```

## 11. 业务主状态机

这一部分是 APP 的核心状态机，包含锁定、输入密码、解锁、修改密码四个状态。

```c

// 业务主状态机逻辑
void APP(u16 ms)
{
	uint8_t matrix_keynum = 0;
	switch(app_status)
	{
		case APP_LOCKED: //未解锁状态 ,即睡眠状态
			matrix_keynum = MK_Get(); //读取键盘的输入
			if(matrix_keynum != 0) //检测是否又按键按下
			{
				/*
				通过键盘唤醒
				*/
				LCD_Clear(WHITE); //屏幕点亮
				app_status = APP_PRE_UNLOCKED;
				LCD_ShowHZ24_Dot(0,200,shu,0);
				LCD_ShowHZ24_Dot(24,200,ru,0);
				LCD_ShowHZ24_Dot(48,200,mi,0);
				LCD_ShowHZ24_Dot(72,200,ma,0);
			}
		break;
		case APP_PRE_UNLOCKED:  // 未解锁状态, 显示输入密码
			if(APP_show_password_text(ms))
			{
				/*
				写解锁成功的逻辑
				*/
				LCD_Clear(WHITE); //刷一下屏幕
//				LCD_ShowString(0,200,320,24,24,"password true!");
				LED_OFF();
				app_status = APP_UNLOCKED;  //状态机切换
			}
		break;
		case APP_UNLOCKED: //解锁状态
			APP_data_update(); //更新数据,非阻塞
			APP_control();  // 检测按键控制,如果有的话
			break;
		case APP_CHANGE_PASSWORD:   // 改变密码的状态
			if(APP_change_password_text(ms))
			{
				LCD_Clear(BLACK); //刷一下屏幕
				/*
				根据赛题发挥,返回睡眠状态或者返回解锁状态
				*/
				app_status = APP_LOCKED;   
			}
			break;
	}
}
```

