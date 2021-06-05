/*
 * c_rtc.cpp
 *
 *  Created on: Jun 1, 2021
 *      Author: xrj
 */


#include "c_rtc.hpp"


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

u32 C_RTC::ReadTimeCounter()
{
	u16 high1 = 0U, high2 = 0U, low = 0U;

	high1 = READ_REG(this->Instance->CNTH & RTC_CNTH_RTC_CNT);
	low   = READ_REG(this->Instance->CNTL & RTC_CNTL_RTC_CNT);
	high2 = READ_REG(this->Instance->CNTH & RTC_CNTH_RTC_CNT);

	if(high1 != high2){
	  low = READ_REG(this->Instance->CNTL & RTC_CNTL_RTC_CNT);
	}
	return (((u32)high1 << 16U) | low);
}

X_State C_RTC::WriteTimeCounter(uint32_t TimeCounter)
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

uint32_t C_RTC::ReadAlarmCounter()
{
	  uint16_t high1 = 0U, low = 0U;

	  high1 = READ_REG(this->Instance->ALRH & RTC_CNTH_RTC_CNT);
	  low   = READ_REG(this->Instance->ALRL & RTC_CNTL_RTC_CNT);

	  return (((uint32_t) high1 << 16U) | low);
}

X_State C_RTC::WriteAlarmCounter(uint32_t AlarmCounter)
{
	X_State status = X_OK;
	if(EnterInitMode() != X_OK){
		status = X_Error;
	}else{
		WRITE_REG(this->Instance->ALRH, (AlarmCounter >> 16U));
		WRITE_REG(this->Instance->ALRL, (AlarmCounter & RTC_ALRL_RTC_ALR));

		/* Wait for synchro */
		if (ExitInitMode() != X_OK){
		  status = X_OK;
		}
	}
	return status;
}

uint16_t C_RTC::ReadPRLL()
{
	return READ_REG(this->Instance->PRLL & RTC_PRLH_PRL);
}

time_t C_RTC::get_ts1970()
{
	return (time_t)ReadTimeCounter() + TIMESTAMP1970_BIAS;
}

X_State C_RTC::set_ts1970(time_t ts)
{
	return WriteTimeCounter(ts - TIMESTAMP1970_BIAS);
}

struct tm* C_RTC::get_tm()
{
	time_t ts = get_ts1970();
	return localtime(&ts);
}

X_State C_RTC::set_tm(struct tm *tm2)
{
	time_t ts = mktime(tm2);
	return set_ts1970(ts);
}
