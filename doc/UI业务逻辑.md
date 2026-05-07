- 直接抄

.c文件

`````c
#include "UI.h"        // 业务
#include "lcd.h"       //显示屏
#include "lcd_hz24.h"   //中文函数
#include "delay.h"      //延时
#include "W25qxx.h"  //存储
#include "Matrix_keyboard.h" //矩阵键盘
#include <string.h>


// 滚动函数
void UI_Scroll(const void *row1[], uint8_t row1_len, const void *row2[], uint8_t row2_len, uint8_t dir, uint16_t interval, uint16_t speed, uint32_t time_ms)
{
	uint8_t i;
	uint16_t w1 = row1_len * interval;
	uint16_t w2 = row2_len * interval;
	uint16_t block_w = w1 > w2 ? w1 : w2;
	int16_t row_gap = (int16_t)(lcddev.height / 4U);
	int16_t row1_y = row_gap;
	int16_t row2_y = row_gap + row_gap;
	int16_t row1_x = (int16_t)((lcddev.width - w1) / 2U);
	int16_t row2_x = (int16_t)((lcddev.width - w2) / 2U);
	int16_t block_h = row_gap + 24;
	int16_t offset;
	uint32_t start = SysTick_GetTick();

	if (dir == 0)offset = -(int16_t)block_w;
	else if (dir == 1)offset = (int16_t)lcddev.width;
	else if (dir == 2)offset = -block_h;
	else offset = (int16_t)lcddev.height;

	while (1)
	{
		uint32_t elapsed = SysTick_GetTick() - start;
		uint16_t wait_ms;

		if (elapsed >= time_ms)break;

		LCD_Clear(WHITE);

		if (dir < 2)
		{
			int16_t base1 = offset + (int16_t)((block_w - w1) / 2U);
			int16_t base2 = offset + (int16_t)((block_w - w2) / 2U);

			for (i = 0; i < row1_len; i++)
			{
				int16_t x = base1 + (int16_t)(i * interval);
				const uint8_t *item = (const uint8_t *)row1[i];

				if (x > -24 && x < (int16_t)lcddev.width)
				{
					if (item[0] >= ' ' && item[0] <= '~' && item[1] == '\0')LCD_ShowChar((u16)x, (u16)row1_y, item[0], 24, 1);
					else LCD_ShowHZ24_Dot((u16)x, (u16)row1_y, item, 1);
				}
			}

			for (i = 0; i < row2_len; i++)
			{
				int16_t x = base2 + (int16_t)(i * interval);
				const uint8_t *item = (const uint8_t *)row2[i];

				if (x > -24 && x < (int16_t)lcddev.width)
				{
					if (item[0] >= ' ' && item[0] <= '~' && item[1] == '\0')LCD_ShowChar((u16)x, (u16)row2_y, item[0], 24, 1);
					else LCD_ShowHZ24_Dot((u16)x, (u16)row2_y, item, 1);
				}
			}
		}
		else
		{
			int16_t y1 = offset;
			int16_t y2 = offset + row_gap;

			if (y1 > -24 && y1 < (int16_t)lcddev.height)
			{
				for (i = 0; i < row1_len; i++)
				{
					int16_t x = row1_x + (int16_t)(i * interval);
					const uint8_t *item = (const uint8_t *)row1[i];

					if (item[0] >= ' ' && item[0] <= '~' && item[1] == '\0')LCD_ShowChar((u16)x, (u16)y1, item[0], 24, 1);
					else LCD_ShowHZ24_Dot((u16)x, (u16)y1, item, 1);
				}
			}

			if (y2 > -24 && y2 < (int16_t)lcddev.height)
			{
				for (i = 0; i < row2_len; i++)
				{
					int16_t x = row2_x + (int16_t)(i * interval);
					const uint8_t *item = (const uint8_t *)row2[i];

					if (item[0] >= ' ' && item[0] <= '~' && item[1] == '\0')LCD_ShowChar((u16)x, (u16)y2, item[0], 24, 1);
					else LCD_ShowHZ24_Dot((u16)x, (u16)y2, item, 1);
				}
			}
		}

		wait_ms = (uint16_t)(time_ms - elapsed);
		if (wait_ms > 20U)wait_ms = 20U;
		delay_ms(wait_ms);

		if (dir == 0)
		{
			offset += (int16_t)speed;
			if (offset > (int16_t)lcddev.width)offset = -(int16_t)block_w;
		}
		else if (dir == 1)
		{
			offset -= (int16_t)speed;
			if (offset < -(int16_t)block_w)offset = (int16_t)lcddev.width;
		}
		else if (dir == 2)
		{
			offset += (int16_t)speed;
			if (offset > (int16_t)lcddev.height)offset = -block_h;
		}
		else
		{
			offset -= (int16_t)speed;
			if (offset < -block_h)offset = (int16_t)lcddev.height;
		}
	}
}

// 开机滚动的函数
void Lcd_bootup_scrolling(void)
{
	// 参数: 第一行字符,长度,第二行字符,长度,滚动方向,字符间距, 速度, 持续时间
	const void *row1[] = {"Z","N","J","J","X","T"};
	const void *row2[] = {"0","2","0","8"};

	UI_Scroll(row1, 6, row2, 4, 0, 24, 12, 3000);
}


// 业务状态枚举
typedef enum{
	APP_CHANGE_PASSWORD, //修改密码状态
	APP_LOCKED, //未解锁状态
	APP_PRE_UNLOCKED, //输入密码的状态
	APP_UNLOCKED, //解锁状态
}APP_STATUS;
	
volatile static APP_STATUS app_status;// 定义业务状态
char blank[] = "                          "; // 清空字符

char default_password[7]; //默认密码

//上电后的业务初始化代码
void APP_init(void)
{
	Lcd_bootup_scrolling(); //开机滚动状态
	app_status = APP_LOCKED; //设置初始状态
	//从flash中读取密码
	W25QXX_Write(default_password,0,7) ;
	// 滚动结束后熄屏
	LCD_Clear(BLACK);
}

// 输入密码的结构体
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
			break;
	}
	password_structure.word_count = count; //已经输入的长度
	return password_structure;
}

// 显示输入密码******的函数,被状态机调用
uint8_t APP_show_password_text(u16 ms)
{ 
	static uint8_t last_len = 0;
	password_st temp = Input_password(ms);
	uint8_t len = temp.word_count; //获取已经输入的密码长度
	if(last_len != len)
	{
		LCD_ShowString(0,250,320,24,24,blank); //清空区域
	}
	for(uint8_t i = 0;i < len;i++)
	{
		LCD_ShowChar(i * 30,250,'*',24,1); //显示*
	}
	last_len = len;
	return temp.is_true; // 返回是否密码正确
}


// 修改密码时的函数,被APP_change_password_text()调用
password_st APP_change_password(void)
{
	static char input[7]; //缓存输入的密码
	static uint8_t count = 0;
	uint8_t num = MK_Get();
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
			for(uint8_t i = 0;i < 7;i++)
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
	password_st temp = APP_change_password();
	uint8_t len = temp.word_count; //获取已经输入的密码长度
	if(last_len != len)
	{
		LCD_ShowString(0,250,320,24,24,blank);
	}
	for(uint8_t i = 0;i < len;i++)
	{
		LCD_ShowChar(i * 30,250,'*',24,1);
	}
	last_len = len;
	return temp.is_true;
}

/*
这里定义显示的数据量
*/
//static uint16_t temp = 0;  //温度
//static uint16_t hum = 0;      // 湿度
//static uint16_t light = 0;           //光照
//static uint8_t motor_status = 0;           //直流电机状态
//static uint8_t led_status = 0;                           // 灯状态

/*
如果有控制模式,定义控制模式
*/
////定义控制模式,默认手动
//static uint8_t control_mode = 0;

// 更新数据,20ms执行一次
void APP_data_update(void)
{
	//处于解锁状态才更新数据
	if(app_status == APP_UNLOCKED)
	{
		/*
		这里写需要获取的所有数据
		*/
//		temp = (uint16_t)Temp_get_t(); //更新温度
//		hum = DHT11_getdata().humidity; //更新湿度
//		light = LDR_LuxData(); //更新光照
//		motor_status = Motor_get_status(); //更新直流电机状态
//		led_status = LED_get_status();
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
//		if(command == 5)
//		{
//			control_mode = ~control_mode; //切换模式
//			return;
//		}
//		if(command == 6) // 修改密码
//		{
//			app_status = APP_CHANGE_PASSWORD; //切换状态
//			LCD_Clear(WHITE); //清屏
//			LCD_ShowString(0,200,320,24,24,"please input password:");
//			return;
//		}
//		if(control_mode == 0)
//		{
//			if(command != 0)
//			{
//				switch(command)
//				{
//					case 1: //开关直流电机
//						if(Motor_get_status()){Motor_set_speed(0);}
//						else{Motor_set_speed(100);}
//						break;
//					case 2:// 控制步进电机
//						Stepmotor_angle_dir(0,90,1);
//						break;
//					case 3:
//						Stepmotor_angle_dir(1,90,1);
//						break;
//					case 4:// LED控制
//						LED_Toggle();
//						break;
//				}
//			}
//		}else
//		{
//			//自动模式
//			if(light < 50)//光线暗时
//			{
//				LED_ON();//开灯
//			}
//			else if(light < 150)
//			{
//				LED_ON();//开灯
//			}
//			else if(light < 200)
//			{
//				LED_OFF();//关灯
//			}
//			else
//			{
//				LED_OFF();//关灯
//			}
//			if(temp > 25)
//			{
//				Motor_set_speed(100); //打开风扇
//			}else
//			{
//				Motor_set_speed(0); //关闭风扇
//			}
//			Stepmotor_light_control(light);
//		}
	}
}

char show_time_str[50];  //显示时间的缓存数组

// 屏幕刷新信息,2s执行一次
void APP_show_info(void)
{
	/*
	比赛的时候发挥,写要刷新的数据
	*/
//	if(Stepmotor_is_run() == 0)// 如果步进电机正在运行,不要刷新,太耗时了
//	{
//		//处于解锁状态才更新数据
//		if(app_status == APP_UNLOCKED)
//		{
//			LCD_Clear(WHITE); //清屏一下
//			LCD_ShowString(0,24,70,24,24,"Temp");
//			LCD_ShowNum(160,24,temp,3,24);
//			LCD_ShowString(250,24,70,24,24,"'C");
//			LCD_ShowString(0,48,70,24,24,"hum");
//			LCD_ShowNum(160,48,hum,3,24);
//			LCD_ShowString(250,48,70,24,24," %");
//			LCD_ShowString(0,72,70,24,24,"Light");
//			LCD_ShowNum(160,72,light,3,24);
//			LCD_ShowString(250,72,70,24,24,"Lux");
//			LCD_ShowString(0,96,70,24,24,"motor");
//			if(motor_status)
//			{
//				LCD_ShowString(160,96,160,24,24,"ON");
//			}else
//			{
//				LCD_ShowString(160,96,160,24,24,"OFF");
//			}
////			LCD_ShowString(0,120,160,24,24,"Curtain");
//	//		LCD_ShowNum(160,120,motor_status,3,24);
//			LCD_ShowString(0,144,160,24,24,"LED:");
//			if(led_status)
//			{
//				LCD_ShowString(160,144,160,24,24,"ON");
//			}else
//			{
//				LCD_ShowString(160,144,160,24,24,"OFF");
//			}
//			LCD_ShowString(0,168,70,24,24,"mode");
//			if(control_mode == 0)
//			{
//				LCD_ShowString(160,168,160,24,24,"Manual mode");
//			}else
//			{
//				LCD_ShowString(160,168,160,24,24,"Auto mode");
//			}
//			LCD_ShowString(0,192,70,24,24,"curtain");
//			LCD_ShowNum(160,192,(uint16_t)Stepmotor_get_curtain_status(),3,24);
//			LCD_ShowString(200,192,160,24,24," %");
//			// 显示时间
//			LCD_ShowString(0,216,70,24,24,"time");
//			sprintf(show_time_str,"%d-%d-%d %d:%d:%d",My_RTC_time[0],My_RTC_time[1],My_RTC_time[2],My_RTC_time[3],My_RTC_time[4],My_RTC_time[5]);
//			LCD_ShowString(120,216,240,24,24,show_time_str);
//		}
//	}
}


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
//				LCD_Clear(WHITE); //屏幕点亮
//				app_status = APP_PRE_UNLOCKED;
//				LCD_ShowHZ24_Dot(0,200,shu,0);
//				LCD_ShowHZ24_Dot(24,200,ru,0);
//				LCD_ShowHZ24_Dot(48,200,mi,0);
//				LCD_ShowHZ24_Dot(72,200,ma,0);
			}
		break;
		case APP_PRE_UNLOCKED:  // 未解锁状态, 显示输入密码
			if(APP_show_password_text(ms))
			{
				/*
				写解锁成功的逻辑
				*/
//				LCD_Clear(WHITE); //刷一下屏幕
////				LCD_ShowString(0,200,320,24,24,"password true!");
//				LED_OFF();
				app_status = APP_UNLOCKED;  //状态机切换
			}
		break;
		case APP_UNLOCKED: //解锁状态
			APP_control();  // 检测按键控制
			break;
		case APP_CHANGE_PASSWORD:   // 改变密码的状态
			if(APP_change_password_text(ms))
			{
				LCD_Clear(WHITE); //刷一下屏幕
				/*
				根据赛题发挥,返回睡眠状态或者返回解锁状态
				*/
				app_status = APP_UNLOCKED;   
			}
			break;
	}
}








`````

.h文件

`````c
#ifndef __UI_H
#define __UI_H		
#include "stm32f4xx.h"

void Lcd_bootup_scrolling(void);//滚动动画
void UI_Scroll(const void *row1[], uint8_t row1_len, const void *row2[], uint8_t row2_len, uint8_t dir, uint16_t interval, uint16_t speed, uint32_t time_ms);
void APP_init(void); //UI初始化

void APP(u16 ms); //APP状态机

// 更新数据,20ms执行一次
void APP_data_update(void);

// 显示信息,2s执行一次
void APP_show_info(void);

// 手动控制的函数
void APP_control(void);


#endif  
	 
	 



`````

