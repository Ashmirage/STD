#include "stm32f4xx.h"
#include "usart.h"
#include <stdio.h>
#include <stdarg.h>

static uint8_t Serial_RxPacket[100];				//接收数据包的缓存数组
uint8_t Serial_RxFlag;					//接收数据包标志位

/**
 * @brief  函数的简要功能描述（一句话）
 *
 * @param[in]   xxx   参数含义，单位，取值范围
 * @param[out]  yyy   输出参数的含义
 * @param[in,out] zzz 输入输出参数说明
 *
 * @return 返回值含义
 *         - 0：成功
 *         - -1：失败（原因）
 *
 * @note   使用注意事项（是否阻塞、是否依赖硬件等）
 * @warning 警告（错误使用会造成什么后果）
 */



/**
 * @brief  串口初始化
 *
 * @param[in]  None
 *
 * @return None
 *
 */
void My_usart_init()
{
	HAL_UART_Receive_IT(&huart1, Serial_RxPacket, sizeof(Serial_RxPacket));
}


/**
 * @brief  串口发送一个字节
 *
 * @param[in]  byte 要发送的字节
 *
 * @return None
 *
 */
void Send_byte(uint8_t byte)
{
	 // 发送 1 字节，超时时间设为最大（一直等）
    HAL_UART_Transmit(&huart1, &byte, 1, HAL_MAX_DELAY);
}

/**
 * @brief  串口发送一个数组
 *
 * @param[in]  array 数组指针
				length 长度
 *
 * @return None
 *
 */
void Send_array(uint8_t* array,uint16_t length)
{
	uint16_t i;
	for(i = 0;i < length;i++)
	{	
		Send_byte(array[i]);
	}
}


/**
 * @brief  串口发送一个字符串
 *
 * @param[in]  str 数组指针
 *
 * @return None
 *
 */
void Send_string(char* str)
{
	uint16_t i = 0;
	while(str[i] != '\0')
	{
		Send_byte(str[i]);
		i++;
	}
}

/**
  * 函    数：次方函数（内部使用）
  * 返 回 值：返回值等于X的Y次方
  */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	
	while (Y --)			
	{
		Result *= X;		
	}
	return Result;
}

/**
  * 函    数：串口发送数字
  * 参    数：Number 要发送的数字，范围：0~4294967295
  * 参    数：Length 要发送数字的长度，范围：0~10
  * 返 回 值：无
  */
void Send_number(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)		
	{
		Send_byte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');	
	}
}

/**
  * 函    数：自己封装的prinf函数
  * 参    数：format 格式化字符串
  * 参    数：... 可变的参数列表
  * 返 回 值：无
  */
void Send_printf(const char* fm,...)
{
	char str[100]; // 缓存数组
	va_list arg; //可变参数列表指针
	va_start(arg,fm); //将列表指针初始化为固定参数后边的可变参数开始位置
	vsprintf(str,fm,arg); //将其打印到缓存数组里面
	va_end(arg); //释放指针
	Send_string(str);
}


// hal库串口1的回调函数,触发中断后自动调用此函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {  
        HAL_UART_Receive_IT(&huart1, Serial_RxPacket, sizeof(Serial_RxPacket));
    }
}

