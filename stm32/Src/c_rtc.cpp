/*
 * c_rtc.cpp
 *
 *  Created on: Jun 1, 2021
 *      Author: xrj
 */


#include "c_rtc.hpp"

#define TIMESTAMP1970_BIAS 946684800  //2000.1.1
uint32_t tz_bias = 8*3600;            //UTC+8 Beijing time
//                       1   2   3   4   5   6   7   8   9  10  11  12
uint8_t DaysOfMon[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#define LEAPS2000(x) ((x)/4-(x)/100+(x)/400+1)

struct tm tm1;

bool IsLeapYear(uint16_t nYear)
{
	  if ((nYear % 4U) != 0U){
	    return 0U;
	  }if((nYear % 100U) != 0U){
	    return 1U;
	  }if((nYear % 400U) == 0U){
	    return 1U;
	  }else{
	    return 0U;
	  }
}

void MyLocalTime(u32 cnt, struct tm* tm)
{
	u32 days = (cnt+tz_bias)/86400;
	tm->tm_wday = (days+5)%7;
	tm->tm_year = days/365;
	if(tm->tm_year>0){
		days -= tm->tm_year*365 + LEAPS2000(tm->tm_year-1);
	}
	bool isLeep;
	goto cleap;
	while(days >= 365+isLeep){
		days-=(365+isLeep);
		tm->tm_year++;
		cleap: isLeep = IsLeapYear(tm->tm_year);
	}
	tm->tm_year += 100;
	tm->tm_yday = days;
	//increase month
	tm->tm_mon = 0;
	tm->tm_mday=1;
	if(isLeep&&days>=60){
		days--;
	}
	u8 *dom = DaysOfMon;
	while(tm->tm_mday+days > *dom){
		tm->tm_mon++;
		days-=*dom++;
	}
	//increase days
	tm->tm_mday = 1+days;

	u32 dsec = (cnt+tz_bias)%86400;
	tm->tm_sec = dsec%60;
	dsec /= 60;
	tm->tm_min = dsec%60;
	tm->tm_hour = dsec/60;
}

u32 MyMkTime(struct tm* tm)
{
	u32 tmp=0;
	if(tm->tm_year>0){
		tmp = tm->tm_year*365 + LEAPS2000(tm->tm_year-1);
	}
	for(int i=0;i<(tm->tm_mon);i++){
		tmp += DaysOfMon[i];
	}
	tmp += tm->tm_mday;
	tmp = tmp*24 + tm->tm_hour;
	tmp = tmp*60 + tm->tm_min;
	tmp = tmp*60 + tm->tm_sec;
	return tmp;
}

#ifdef USE_TIME_H
void TimeH_tz()
{
	setenv("TZ", "GMT-8", 1);
	tzset();
}
#endif

//copy from "stm32f1xx_hal_rtc.c"
X_State C_RTC::EnterInitMode()
{
  uint32_t tickstart = 0U;

  tickstart = HAL_GetTick();
  /* Wait till RTC is in INIT state and if Time out is reached exit */
  while ((this->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET){
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE){
      return X_Timeout;
    }
  }

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(this);

  return X_OK;
}

X_State C_RTC::ExitInitMode()
{
  uint32_t tickstart = 0U;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(this);

  tickstart = HAL_GetTick();
  /* Wait till RTC is in INIT state and if Time out is reached exit */
  while ((this->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE){
      return X_Timeout;
    }
  }
  return X_OK;
}

X_State C_RTC::wait_sync()
{
	return (X_State)HAL_RTC_WaitForSynchro(this);
}

u32 C_RTC::get_cnt()
{
	u16 high1 = 0U, high2 = 0U, low = 0U;

	high1 = READ_REG(this->Instance->CNTH & RTC_CNTH_RTC_CNT);
	low   = READ_REG(this->Instance->CNTL & RTC_CNTL_RTC_CNT);
	high2 = READ_REG(this->Instance->CNTH & RTC_CNTH_RTC_CNT);

	if(high1 != high2){
	  low = READ_REG(this->Instance->CNTL & RTC_CNTL_RTC_CNT);
	}
	return (((u32)high2 << 16U) | low);
}

X_State C_RTC::set_cnt(uint32_t TimeCounter)
{
	X_State status = X_OK;
	if(EnterInitMode() != X_OK){
		status = X_Error;
	}else{
		WRITE_REG(this->Instance->CNTH, (TimeCounter >> 16U));
		WRITE_REG(this->Instance->CNTL, (TimeCounter & RTC_CNTL_RTC_CNT));
		if (ExitInitMode() != X_OK){
			status = X_Error;
		}
	}
	return status;
}

uint32_t C_RTC::get_alarm_cnt()
{
	  uint16_t high1 = 0U, low = 0U;

	  high1 = READ_REG(this->Instance->ALRH & RTC_CNTH_RTC_CNT);
	  low   = READ_REG(this->Instance->ALRL & RTC_CNTL_RTC_CNT);

	  return (((uint32_t) high1 << 16U) | low);
}

X_State C_RTC::set_alarm_cnt(uint32_t AlarmCounter, bool IT)
{
	X_State status = X_OK;
	if(EnterInitMode() != X_OK){
		status = X_Error;
	}else{
		WRITE_REG(this->Instance->ALRH, (AlarmCounter >> 16U));
		WRITE_REG(this->Instance->ALRL, (AlarmCounter & RTC_ALRL_RTC_ALR));

		/* Wait for synchro */
		if (ExitInitMode() != X_OK){
		  status = X_Error;
		}
	}
	if(IT && status == X_OK){
		__HAL_RTC_ALARM_CLEAR_FLAG(this, RTC_FLAG_ALRAF);
		__HAL_RTC_ALARM_ENABLE_IT(this, RTC_IT_ALRA);
		__HAL_RTC_ALARM_EXTI_CLEAR_FLAG();
		HAL_NVIC_ClearPendingIRQ(RTC_IRQn);
		__HAL_RTC_ALARM_EXTI_ENABLE_IT();
		__HAL_RTC_ALARM_EXTI_ENABLE_RISING_EDGE();
	}
	return status;
}

X_State C_RTC::set_alarm_sec(u32 sec, bool IT)
{
	u32 cnt = get_cnt();
	return set_alarm_cnt(cnt+sec, IT);
}

//subsecons of timestamp = (32767-divl)/32768
uint16_t C_RTC::get_divl()
{
	return READ_REG(this->Instance->DIVL);
}

time_t C_RTC::get_ts1970()
{
	return (time_t)get_cnt() + TIMESTAMP1970_BIAS;
}

X_State C_RTC::set_ts1970(time_t ts)
{
	return set_cnt(ts - TIMESTAMP1970_BIAS);
}

struct tm* C_RTC::get_tm()
{
#ifdef USE_TIME_H
	time_t ts = get_ts1970();
	return localtime(&ts);
#else
	u32 cnt = get_cnt();
	MyLocalTime(cnt, &tm1);
	return &tm1;
#endif
}

X_State C_RTC::set_tm(struct tm *tm2)
{
#ifdef USE_TIME_H
	time_t ts = mktime(tm2);
	return set_ts1970(ts);
#else
	u32 cnt = MyMkTime(tm2);
	return set_cnt(cnt);
#endif
}
