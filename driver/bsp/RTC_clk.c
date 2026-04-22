#include "stm32f4xx.h"
#include "RTC_clk.h"

#define RTC_INIT_FLAG 0x6666U

volatile uint16_t My_RTC_time[] = {2026, 4, 21, 8, 47, 0};

static uint8_t rtc_ready = 0;

/* 把用户使用的年份转换成 RTC 硬件里的 0~99 年份字段。 */
static uint8_t rtc_year_to_hw(uint16_t year)
{
    if (year >= 2000U)
    {
        return (uint8_t)(year - 2000U);
    }

    return (uint8_t)year;
}

/* 根据年月日计算 RTC 需要的星期字段。 */
static uint8_t rtc_get_weekday(uint16_t year, uint8_t month, uint8_t day)
{
    static const uint8_t month_code[] = {0U, 3U, 2U, 5U, 0U, 3U, 5U, 1U, 4U, 6U, 2U, 4U};
    uint16_t calc_year = year;
    uint8_t weekday;

    if (month < 3U)
    {
        calc_year--;
    }

    weekday = (uint8_t)((calc_year + calc_year / 4U - calc_year / 100U +
                         calc_year / 400U + month_code[month - 1U] + day) % 7U);

    return (weekday == 0U) ? RTC_Weekday_Sunday : weekday;
}

/* 把 My_RTC_time 写入 RTC，同时刷新备份寄存器初始化标记。 */
static void rtc_apply_time_from_array(void)
{
    RTC_InitTypeDef rtc_init;
    RTC_TimeTypeDef rtc_time;
    RTC_DateTypeDef rtc_date;

    RTC_StructInit(&rtc_init);
    rtc_init.RTC_HourFormat = RTC_HourFormat_24;
    rtc_init.RTC_AsynchPrediv = 127U;
    rtc_init.RTC_SynchPrediv = 255U;
    RTC_Init(&rtc_init);

    RTC_TimeStructInit(&rtc_time);
    rtc_time.RTC_Hours = (uint8_t)My_RTC_time[3];
    rtc_time.RTC_Minutes = (uint8_t)My_RTC_time[4];
    rtc_time.RTC_Seconds = (uint8_t)My_RTC_time[5];
    RTC_SetTime(RTC_Format_BIN, &rtc_time);

    RTC_DateStructInit(&rtc_date);
    rtc_date.RTC_Year = rtc_year_to_hw(My_RTC_time[0]);
    rtc_date.RTC_Month = (uint8_t)My_RTC_time[1];
    rtc_date.RTC_Date = (uint8_t)My_RTC_time[2];
    rtc_date.RTC_WeekDay = rtc_get_weekday(My_RTC_time[0], (uint8_t)My_RTC_time[1], (uint8_t)My_RTC_time[2]);
    RTC_SetDate(RTC_Format_BIN, &rtc_date);

    RTC_WriteBackupRegister(RTC_BKP_DR0, RTC_INIT_FLAG);
}

/* 从当前 RTC 日历寄存器读取时间到 My_RTC_time。 */
static void rtc_sync_array_from_hw(void)
{
    RTC_TimeTypeDef rtc_time;
    RTC_DateTypeDef rtc_date;

    RTC_GetTime(RTC_Format_BIN, &rtc_time);
    RTC_GetDate(RTC_Format_BIN, &rtc_date);

    My_RTC_time[0] = (uint16_t)(rtc_date.RTC_Year + 2000U);
    My_RTC_time[1] = rtc_date.RTC_Month;
    My_RTC_time[2] = rtc_date.RTC_Date;
    My_RTC_time[3] = rtc_time.RTC_Hours;
    My_RTC_time[4] = rtc_time.RTC_Minutes;
    My_RTC_time[5] = rtc_time.RTC_Seconds;
}

/* 初始化 RTC/LSE，只做一次；首次上电时写入默认时间。 */
void RTC_clk_init(void)
{
    if (rtc_ready != 0U)
    {
        return;
    }

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    if (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
        RCC_LSEConfig(RCC_LSE_ON);
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
        {
        }
    }

    if ((RCC->BDCR & RCC_BDCR_RTCSEL) == 0U)
    {
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    }

    RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForSynchro();

    /* 首次上电时写入默认时间，并留下备份域标记。 */
    if (RTC_ReadBackupRegister(RTC_BKP_DR0) != RTC_INIT_FLAG)
    {
        rtc_apply_time_from_array();
    }

    rtc_sync_array_from_hw();
    rtc_ready = 1U;
}

/* 把 My_RTC_time 里的值强制写入 RTC。 */
void My_RTC_settime(void)
{
    RTC_clk_init();
    rtc_apply_time_from_array();
}

/* 从 RTC 实时寄存器刷新 My_RTC_time。 */
void My_RTC_readtime(void)
{
    RTC_clk_init();
    rtc_sync_array_from_hw();
}
