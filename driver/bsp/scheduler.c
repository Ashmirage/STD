#include "stm32f4xx.h"
#include "delay.h"
#include "lcd.h"
#include "usart.h"
#include  "touch.h"
#include "Matrix_keyboard.h"
#include "servo.h"
#include "w25qxx.h"
#include "Pot.h"
//#include "led.h"
//#include "my_usart.h"
#include "Relay.h"
//#include "Buzzer.h"
//#include "delay_us.h"
//#include "lcd.h"
//#include "UI.h"
//#include "lcd_font.h"
#include "remote.h"
//#include "HW.h"
//#include "24cxx.h"
//#include "AD.h"
//#include "LDR.h"
#include "Motor.h"
//#include "stepmotor.h"
//#include "UI.h"
#include "RTC_clk.h"
//#include "syn6288.h"
//#include "usart.h"
// 1ms执行一次
static void Loop_1000hz(void)
{

//	LED_tick();
//	APP_data_update(); //信息采集
//	Stepmotor_Rhythm_1ms(); // 步进电机
//	LDR_ADC_Read_1ms(); //光敏ADC采样读取
}

// 20ms执行一次
float angle = 0;
int8_t dir = 1;
static void Loop_50hz(void)
{
	uint8_t num = MK_Get();
//	uint8_t command = remote_scan();
	if(num != 0){
		angle += dir*90;
		if(angle >= 180){
			dir = -dir;
		}else if(angle <= 0){
			dir = -dir;
		}
		Send_printf("num=%d\r\n",num);
		Servo_SetAngle(angle);
	}
	Send_printf("pot=%d\r\n",Pot_GetData()) ;
//	if(command != 0){
//		Send_printf("command=%d\r\n",command);
//	}
//	if (tp_dev.scan(0))  // 正常扫描，电容屏就这么用
//    {
//		Send_printf("touch\r\n");
////        uint16_t x = tp_dev.x[0];
////        uint16_t y = tp_dev.y[0];
////		TP_Draw_Big_Point(x,y,RED);
////		Send_printf("x=%d,y=%d\r\n",x,y);
//        // x,y 就是当前第 1 个触点的屏幕坐标
//    }
//	APP(20); //APP业务
//	Buzzer_alarm(20); //蜂鸣器报警
////	Send_printf("lux=%d\r\n",light_data);
}

// 500ms执行一次
static void Loop_2hz(void)
{
	
}

uint8_t status = 0;
static void Loop_1hz(void)
{
	
	uint8_t tx[] = "123456";
    uint8_t rx[sizeof(tx)] = {0};
	Relay_status(status);
	status = 1 - status;
    W25QXX_Write(tx, 0x000000, sizeof(tx));   // 写到 0 地址
    W25QXX_Read(rx, 0x000000, sizeof(rx));    // 从 0 地址读回

    Send_printf("rx = %s\r\n", rx);
//	 speed += dir * 20;
//	if(speed >= 100){
//		dir = -dir;
//	}else if(speed <= -100){
//		dir = -dir;
//	}
//	Motor_set_speed(speed);
//	Send_printf("speed=%d~\r\n",speed);
	GPIO_ToggleBits(GPIOF,GPIO_Pin_10);
//	LED_Toggle();
//	x++;g
//	if(x % 2 == 0){
//		APP_show_info();
//	}
//	My_RTC_readtime();
//	Send_printf("%d-%d-%d %d-%d-%d\r\n",My_RTC_time[0],My_RTC_time[1],My_RTC_time[2],My_RTC_time[3],My_RTC_time[4],My_RTC_time[5]);
//	DHT11_update_data();//DHT11数据读取,这是阻塞式的,25ms左右
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
	uart_init(115200);
//	My_usart_init(); //串口初始化
//	Matrix_keyboard_init(); //矩阵键盘初始化
//	Buzzer_init(); //蜂鸣器初始化
	LCD_Init(); //LCD显示屏初始化
	TP_Init();
	MK_Init();
	LCD_Clear(WHITE);
	LCD_ShowString(0,0,200,24,24,"123456");
	Motor_init();
	RTC_clk_init();
	remote_init(); // 红外遥控初始化
	Servo_Init(); //舵机初始化
	W25QXX_Init();
    Send_printf("W25Q ID = 0x%04X\r\n", W25QXX_TYPE);
	Pot_Init(); //电位器 ADC 初始化，当前用 PA5(STM_ADC)
	Relay_init(); //继电器初始化，默认保持断开
//	AD_init(); //AD转换初始化
//	Motor_init();//直流电机初始化
//	STEPMOTOR_Init(); //步进电机初始化
//	at24cxx_init(); //FLASH初始化
//	APP_init(); //开机动画
//	My_RTC_settime();
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
//	APP_init(); //开机动画
////	Send_printf("start=%d\r\n",SysTick_GetTick());
////	struct DHT11_data data = DHT11_read_data();
////	if(data.status == DHT11_DATA_OK)
////	{
////		Send_printf("end=%d\r\n,hum=%d",SysTick_GetTick(),data.humidity);
////	}
}
