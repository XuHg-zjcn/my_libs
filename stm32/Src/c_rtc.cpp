/*
 * c_rtc.cpp
 *
 *  Created on: Jun 1, 2021
 *      Author: xrj
 */


#include "c_rtc.hpp"

struct tm tm1;

extern uint32_t RTC_ReadTimeCounter(RTC_HandleTypeDef*);
extern HAL_StatusTypeDef RTC_WriteTimeCounter(RTC_HandleTypeDef*, uint32_t);


void C_RTC::HAL2tm(struct tm* tm2, RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime)
{
	tm2->tm_year = (int)sDate->Year+100;
	tm2->tm_mon = sDate->Month-1;
	tm2->tm_mday = sDate->Date;
	tm2->tm_wday = sDate->WeekDay;
	tm2->tm_hour = sTime->Hours;
	tm2->tm_min = sTime->Minutes;
	tm2->tm_sec = sTime->Seconds;
}

void C_RTC::tm2HAL(struct tm* tm2, RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime)
{
	sDate->Year = tm2->tm_year-100;
	sDate->Month = tm2->tm_mon+1;
	sDate->Date = tm2->tm_mday;
	sDate->WeekDay = tm2->tm_wday;
	sTime->Hours = tm2->tm_hour;
	sTime->Minutes = tm2->tm_min;
	sTime->Seconds = tm2->tm_sec;
}

struct tm* C_RTC::get_tm()
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	HAL_RTC_GetTime(this, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(this, &sDate, RTC_FORMAT_BIN);
	HAL2tm(&tm1, &sDate, &sTime);
	return &tm1;
}

void C_RTC::set_tm(struct tm *tm2)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	tm2HAL(tm2, &sDate, &sTime);
	HAL_RTC_SetTime(this, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(this, &sDate, RTC_FORMAT_BIN);
}

time_t C_RTC::get_timestamp()
{
	return mktime(get_tm());
}

void C_RTC::set_timestamp(time_t ts)
{
	struct tm *tm2 = localtime(&ts);
	set_tm(tm2);
}

u32 C_RTC::get_cnt()
{
	  return RTC_ReadTimeCounter(this);
}

void C_RTC::set_cnt(u32 cnt)
{
	RTC_WriteTimeCounter(this, cnt);
}

void C_RTC::SetAlarm(RTC_TimeTypeDef *sTime, uint32_t Format, bool IT)
{
	RTC_AlarmTypeDef sAlarm;
	sAlarm.AlarmTime = *sTime;
	sAlarm.Alarm = RTC_ALARM_A;
	if(IT){
		HAL_RTC_SetAlarm_IT(this, &sAlarm, Format);
	}else{
		HAL_RTC_SetAlarm(this, &sAlarm, Format);
	}
}

void C_RTC::GetAlarm(RTC_TimeTypeDef *sTime, uint32_t Format)
{
	RTC_AlarmTypeDef sAlarm;
	HAL_RTC_GetAlarm(this, &sAlarm, RTC_ALARM_A, Format);
	*sTime = sAlarm.AlarmTime;
}
