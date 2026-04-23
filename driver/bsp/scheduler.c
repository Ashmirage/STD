#include "stm32f4xx.h"
#include "delay.h"  //延时函数
#include "lcd.h"    //TFT屏幕
#include "usart.h"  //串口调试
#include "touch.h"  //触摸功能
#include "Matrix_keyboard.h" //矩阵键盘
#include "servo.h" //舵机
#include "w25qxx.h" //存储芯片
#include "AD.h"   // 三个ADC采样
#include "HCSR04.h"  //超声波
#include "DHT11.h"   //温湿度
#include "lcd_font.h"  //中文字库
#include "lcd_hz24.h"  // 显示中文的函数
#include "UI.h"      // 业务
#include "Relay.h"   //继电器
#include "remote.h"  //遥控器
#include "Motor.h"     //直流电机
#include "RTC_clk.h"    //实时时钟
extern void io0(void);//三个IO初始化
extern void gt(void); // 绿色LED翻转
extern void rt(void); //红色LED翻转
extern void bt(void); //蜂鸣器翻转

// 1ms执行一次
static void Loop_1000hz(void)
{
//	APP_data_update(); //信息采集
}

// 20ms执行一次
static void Loop_50hz(void)
{

}

// 500ms执行一次
static void Loop_2hz(void)
{
	gt();  //绿灯翻转
	rt();  //红灯翻转
}

// 1s执行一次
static void Loop_1hz(void)
{

}

// 定义执行任务的结构体
typedef struct
{
	void (*task_func)(void); //任务函数的指针
	uint32_t task_hz; //任务的执行频率
	uint16_t interval_ticks; //任务执行的间隔tick数
	uint32_t last_runtime; //任务上次执行的时间
}scheduler_task;


// 定义所有要执行的任务,一个数组
scheduler_task task_array[] = {
	{Loop_1000hz,1000,0,0},
	{Loop_50hz,50,0,0},
	{Loop_2hz,2,0,0},
	{Loop_1hz,1,0,0},
};

// 裸机任务调度器初始化
void Scheduler_init(void)
{
	uint8_t i;
	for(i = 0;i < 4;i++)
	{
		task_array[i].interval_ticks = (uint16_t)1000 / task_array[i].task_hz; //计算每个任务多少个tick执行一次 T * 1000 = 1000 / f
		if(task_array[i].interval_ticks < 1) // 如果执行tick间隔小于1,至少应该为1
		{
			task_array[i].interval_ticks = 1;
		}
	}
}

// 主循环里面不断循环执行这个函数
void Scheduler_run(void)
{
	uint8_t index;
	uint32_t now_t = SysTick_GetTick(); // 一次读取，所有任务共用
	// 轮询检查不同周期的函数是否需要执行
	for(index = 0;index < 4;index++)
	{
		if((now_t - task_array[index].last_runtime) >= task_array[index].interval_ticks)
		{
			// 如果当前间隔数>=任务间隔数
			task_array[index].last_runtime = now_t; //更新该任务上次执行的时间
			task_array[index].task_func(); // 执行该任务
		}
	}
}
void Hardware_init(void)
{
	uart_init(115200);//串口调试
	LCD_Init(); //LCD显示屏
	TP_Init();  //触摸
	MK_Init();  //矩阵键盘
	Motor_init(); //直流电机初始化
	RTC_clk_init();  //实时时钟初始化
	io0(); // PF8 蜂鸣器，PF9/PF10 LED
	remote_init(); // 红外遥控初始化
	Servo_Init(); //舵机初始化
	W25QXX_Init();   //存储芯片初始化
	AD_Init(); //AD 初始化
	HCSR04_Init(); //超声波初始化，当前默认 PE1/PE3 + TIM5
	DHT11_Init(); //DHT11 初始化，当前默认 PC6
	Relay_init(); //继电器初始化，默认保持断开
	Lcd_bootup_scrolling();  //滚动字幕);化
}
