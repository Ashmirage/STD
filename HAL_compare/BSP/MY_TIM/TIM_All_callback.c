#include "tim.h"
#include "remote.h"

// 定时器更新溢出中断
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// 判断是哪个定时器
    if (htim->Instance == TIM1)
    {
		Remote_TIM_ElapsedCallback();
    }
}

//输入捕获中断
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
		// 判断是哪个定时器
    if (htim->Instance == TIM1)
    {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			Remote_IC_CaptureCallback();
		}
    }
}


















