#include "stm32f4xx.h"
#include "led.h"
#include "my_usart.h"
#include "Matrix_keyboard.h"
#include "Relay.h"
#include "Buzzer.h"
#include "delay_us.h"
#include "lcd.h"
#include "UI.h"
#include "lcd_font.h"
#include "remote.h"
#include "HW.h"
#include "24cxx.h"
#include "AD.h"
#include "LDR.h"
#include "Motor.h"
#include "stepmotor.h"
#include "UI.h"
#include "RTC_clk.h"
#include "syn6288.h"
#include "usart.h"


//uint8_t MID;							//定义用于存放MID号的变量
//uint16_t DID;							//定义用于存放DID号的变量

//uint8_t ArrayWrite[] = {0x01, 0x02, 0x03, 0x04};	//定义要写入数据的测试数组
//uint8_t ArrayRead[7];								//定义要读取数据的测试数组

#define TASK_NUM 6
// 1ms执行一次
static void Loop_1000hz(void)
{
	LED_tick();
	APP_data_update(); //信息采集
	Stepmotor_Rhythm_1ms(); // 步进电机
	LDR_ADC_Read_1ms(); //光敏ADC采样读取
}

// 2ms执行一次
static void Loop_500hz(void)
{
	
}

// 5ms执行一次
static void Loop_200hz(void)
{
	
}

#include "pot.h"
#include "Temp.h"
// 20ms执行一次
static void Loop_50hz(void)
{
	APP(20); //APP业务
	Buzzer_alarm(20); //蜂鸣器报警
//	Send_printf("lux=%d\r\n",light_data);
}

// 500ms执行一次
uint8_t mid;
uint16_t did;
int16_t speed = 0;
int8_t dir = 1;
static void Loop_2hz(void)
{
	  speed += dir * 20;
	if(speed >= 100){
		dir = -dir;
	}else if(speed <= -100){
		dir = -dir;
	}
	Motor_set_speed(speed);
	Send_printf("speed=%d~\r\n",speed);
//	Send_printf("running~\r\n");
}

#include "DHT11.h"
// 1s执行一次
//const uint8_t write[] = {0x12,0x34,0x56};
//uint8_t read[] = {0x00,0x00,0x00};
uint32_t x = 0;
static void Loop_1hz(void)
{
	   LED_Toggle();
	x++;
	if(x % 2 == 0){
		APP_show_info();
	}
	My_RTC_readtime();
	DHT11_update_data();//DHT11数据读取,这是阻塞式的,25ms左右
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
	{Loop_500hz,500,0,0},
	{Loop_200hz,200,0,0},
	{Loop_50hz,50,0,0},
	{Loop_2hz,2,0,0},
	{Loop_1hz,1,0,0},
};

// 裸机任务调度器初始化
void Scheduler_init(void)
{
	uint8_t i;
	for(i = 0;i < TASK_NUM;i++)
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
	uint32_t now_t = HAL_GetTick(); // 一次读取，所有任务共用
	// 轮询检查不同周期的函数是否需要执行
	for(index = 0;index < TASK_NUM;index++)
	{
		if((now_t - task_array[index].last_runtime) >= task_array[index].interval_ticks)
		{
			// 如果当前间隔数>=任务间隔数
			task_array[index].last_runtime = now_t; //更新该任务上次执行的时间
			task_array[index].task_func(); // 执行该任务
		}
	}
}

//选择背景音乐2。(0：无背景音乐  1-15：背景音乐可选)
		//m[0~16]:0背景音乐为静音，16背景音乐音量最大
		//v[0~16]:0朗读音量为静音，16朗读音量最大
		//t[0~5]:0朗读语速最慢，5朗读语速最快
		//其他不常用功能请参考数据手册
// 各种外设,驱动的初始化函数
void Hardware_init(void)
{
	DWT_Init();
	My_usart_init(); //串口初始化
	Matrix_keyboard_init(); //矩阵键盘初始化
	Buzzer_init(); //蜂鸣器初始化
	LCD_Init(); //LCD显示屏初始化
	Relay_init(); //继电器初始化
	AD_init(); //AD转换初始化
	Motor_init();//直流电机初始化
	STEPMOTOR_Init(); //步进电机初始化
	at24cxx_init(); //FLASH初始化
//	W25Q128_SectorErase(PASSWORD_ADDRESS);
//	W25Q128_PageProgram(PASSWORD_ADDRESS,"123456",7);
	APP_init(); //开机动画
//	Send_printf("???\r\n");
	My_RTC_settime();
	SYN6288_Init(&huart6);
	static const char syn6288_demo_text_utf8[] =
		"[v7][m1][t5]你不好你不好你不好";
  (void)SYN_FrameInfoUtf8(2U, syn6288_demo_text_utf8);
//	Send_printf("11111");
	//暂停合成，此时没有用到，用于展示函数用法
	//YS_SYN_Set(SYN_SuspendCom);
//	remote_init(); //红外遥控器初始化
//	CountSensor_Init(); //对射式红外传感器初始化
//	HW_Init(); //光电红外传感器初始化
//	at24cxx_init(); //EEPROM初始化
//	My_RTC_settime(); // rtc实时始终设置时间
//	Matrix_keyboard_init(); //矩阵键盘初始化
//	Relay_init(); //继电器初始化
//	Buzzer_init();  // 蜂鸣器初始化
//	My_usart_init(115200); //串口初始化
//	TIM_it_init(); //定时器初始化
//	Matrix_keyboard_init(); //矩阵键盘初始化
//	LED_init(); //LED初始化
//	Buzzer_init(); //蜂鸣器初始化
//	while (at24cxx_check()) /* 检测不到24c02 */
//    {
//        Send_printf("24C02 Check Failed!\r\n");
//        HAL_Delay(500);
//    }
//	Send_printf("check success!\r\n");
//	HAL_Delay(500);
//	at24cxx_write(0,write,3);
//	Lcd_bootup_scrolling(); // 
//	Relay_init(); //继电器初始化
//	Motor_init();//直流电机初始化
//	STEPMOTOR_Init(); //步进电机初始化
//	W25Q128_Init(); //FLASH初始化
//	RTC_clk_init(); //RTC初始化
////	W25Q128_SectorErase(PASSWORD_ADDRESS);
////	W25Q128_PageProgram(PASSWORD_ADDRESS,"123456",7);
//	APP_init(); //开机动画
//	
////	Send_printf("start=%d\r\n",SysTick_GetTick());
////	struct DHT11_data data = DHT11_read_data();
////	if(data.status == DHT11_DATA_OK)
////	{
////		Send_printf("end=%d\r\n,hum=%d",SysTick_GetTick(),data.humidity);
////	}
}







