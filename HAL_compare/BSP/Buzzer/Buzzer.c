#include "stm32f4xx.h"

#define Buzzer_PORT GPIOF
#define Buzzer_PIN GPIO_PIN_8

void Buzzer_init(void)
{
	HAL_GPIO_WritePin(Buzzer_PORT,Buzzer_PIN,GPIO_PIN_RESET);
}

// 注意区分是否是高电平点亮
void Buzzer_ON(void)
{
	HAL_GPIO_WritePin(Buzzer_PORT,Buzzer_PIN,GPIO_PIN_SET);
}

void Buzzer_OFF(void)
{
	HAL_GPIO_WritePin(Buzzer_PORT,Buzzer_PIN,GPIO_PIN_RESET);
}

static uint8_t alarm_mode = 0; //默认非报警模式

void Buzzer_alarm_start(void)
{
	alarm_mode = 1;
}

void Buzzer_alarm(uint16_t ms)
{
	if(alarm_mode == 1)//如果是报警模式
	{
		static uint8_t status = 0;
		static uint32_t count = 0;
		switch(status)
		{
			case 0://最短鸣叫
				count++;
				Buzzer_ON();
				if(count * ms >= 150)
				{
					Buzzer_OFF();
					count = 0;
					status = 1;
				}
			break;
			case 1:
				count++;
				if(count * ms >= 200)
				{
					count = 0;
					status = 2;
				}
			break;
			case 2:
				count++;
				Buzzer_ON();
				if(count * ms >= 300)
				{
					Buzzer_OFF();
					count = 0;
					status = 3;
				}
			break;
			case 3:
				count++;
				if(count * ms >= 200)
				{
					count = 0;
					status = 4;
				}
			break;
			case 4:
				count++;
				Buzzer_ON();
				if(count * ms >= 450)
				{
					Buzzer_OFF();
					count = 0;
					status = 0;
					alarm_mode = 0;
				}
			break;
		}
	}
}


