#include "stm32f4xx.h"
#include <stdio.h>

#define ROW1_GPIO_PORT GPIOE //row1的端口
#define ROW2_GPIO_PORT GPIOE //row2的端口
#define ROW3_GPIO_PORT GPIOE //row3的端口
#define ROW4_GPIO_PORT GPIOE //row4的端口
#define ROW1_GPIO_PIN GPIO_PIN_6 //row1的引脚
#define ROW2_GPIO_PIN GPIO_PIN_4 //row2的引脚
#define ROW3_GPIO_PIN GPIO_PIN_2 //row3的引脚
#define ROW4_GPIO_PIN GPIO_PIN_0//row4的引脚

#define COL1_GPIO_PORT GPIOC //col1的端口
#define COL2_GPIO_PORT GPIOC //col2的端口
#define COL3_GPIO_PORT GPIOC //col3的端口
#define COL4_GPIO_PORT GPIOC //col4的端口
#define COL1_GPIO_PIN GPIO_PIN_0 //col1的引脚
#define COL2_GPIO_PIN GPIO_PIN_1 //col2的引脚
#define COL3_GPIO_PIN GPIO_PIN_2 //col3的引脚
#define COL4_GPIO_PIN GPIO_PIN_3 //col4的引脚

#define KEY_DEBOUNCE_MS 20U

// 结构体定义
struct Matrix_port_pin
{
	GPIO_TypeDef* GPIO_port;
	uint16_t GPIO_pin;
};

//端口数组
static struct Matrix_port_pin ROW_port_pin[4] = {
	{ROW1_GPIO_PORT,ROW1_GPIO_PIN},{ROW2_GPIO_PORT,ROW2_GPIO_PIN},{ROW3_GPIO_PORT,ROW3_GPIO_PIN},{ROW4_GPIO_PORT,ROW4_GPIO_PIN}
};

static struct Matrix_port_pin COL_port_pin[4] = {
	{COL1_GPIO_PORT,COL1_GPIO_PIN},{COL2_GPIO_PORT,COL2_GPIO_PIN},{COL3_GPIO_PORT,COL3_GPIO_PIN},{COL4_GPIO_PORT,COL4_GPIO_PIN}
};

void ROW_W(struct Matrix_port_pin row_port_pin,uint8_t x)
{
	HAL_GPIO_WritePin(row_port_pin.GPIO_port,row_port_pin.GPIO_pin,(GPIO_PinState)x);
}
uint8_t COL_R(struct Matrix_port_pin col_port_pin)
{
	return HAL_GPIO_ReadPin(col_port_pin.GPIO_port,col_port_pin.GPIO_pin);
}

/**
 * @brief  矩阵键盘初始化
 *
 * @param[in]  None
 *
 * @return None
 *
 */
void Matrix_keyboard_init(void)
{
	// 全部拉低
	for(uint8_t i = 0;i < 4;i++)
	{
		ROW_W(ROW_port_pin[i],0);
	}
}


/**
 * @brief  矩阵键盘获取键盘数字
 *
 * @param[in]  None
 *
 * @return 按键的数字1-16
 *
 */
uint8_t Matrix_number_check(void)
{
	uint8_t key_num = 0;
	for(uint8_t row = 0;row < 4;row++)
	{
		for(uint8_t i = 0;i < 4;i++)
		{
			ROW_W(ROW_port_pin[i],0);//先手动全部拉低
		}
		ROW_W(ROW_port_pin[row],1); //将要扫描的行拉高
		for(uint8_t col = 0;col < 4;col++)
		{
			if(COL_R(COL_port_pin[col]) == 1)
			{
				key_num = row * 4 + col + 1;
				ROW_W(ROW_port_pin[row],0); //退出函数前恢复原状
				return key_num;
			}
		}
		ROW_W(ROW_port_pin[row],0);
	}
	return key_num;
}


// 消抖+获取数字
uint8_t Matrix_keyboard_get_number(void)
{
	static uint8_t status;
	static uint8_t pre_num = 0;
	static uint8_t cur_num = 0;
	static uint32_t start_t;
	static uint8_t candidate;
	pre_num = cur_num; //更新上次的键值
	cur_num = Matrix_number_check(); //获取本次的键值
	switch(status)
	{
		case 0:
			if(pre_num == cur_num && pre_num != 0)
			{
				status = 1;
				start_t = HAL_GetTick(); //记录按下的时间
			}
			break;
		case 1: //消抖状态
			if(pre_num == cur_num)
			{
				if((HAL_GetTick() - start_t) > 10) //消抖10ms
				{
					candidate = cur_num; //缓存键值
					status = 2;
				}
			}else // 不一致,视为噪声
			{
				status = 0;
			}
			break;
		case 2:
			if(pre_num == cur_num && pre_num != 0)
			{
				//等待松手
			}else
			{
				status = 0;
				return candidate; //松手后返回这次的键值
			}
			break;
	}
	return 0;
}

