#include "stm32f4xx.h"
#include "RTC_clk.h"
#include "rtc.h"

#define RTC_INIT_FLAG 0x7777

volatile uint16_t My_RTC_time[] = {26,4,18,13,47,30};



void My_RTC_settime(void)
{
	// 判断是否需要初始化,读取bkp的数据
	if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != RTC_INIT_FLAG)
    {
        RTC_TimeTypeDef sTime = {0};
        RTC_DateTypeDef sDate = {0};

        // 设置时间
        sTime.Hours = My_RTC_time[3];
        sTime.Minutes = My_RTC_time[4];
        sTime.Seconds = My_RTC_time[5];

        HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

        // 设置日期
        sDate.Year = My_RTC_time[0];
        sDate.Month = My_RTC_time[1];
        sDate.Date = My_RTC_time[2];

        HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        // 写标志位
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, RTC_INIT_FLAG);
    }
}

void My_RTC_readtime(void)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	My_RTC_time[0] = sDate.Year+2000; // Year变量只能存储0-99,因此需要补足2000
	My_RTC_time[1] = sDate.Month;
	My_RTC_time[2] = sDate.Date;
	My_RTC_time[3] = sTime.Hours;
	My_RTC_time[4] = sTime.Minutes;
	My_RTC_time[5] = sTime.Seconds;
	
}


