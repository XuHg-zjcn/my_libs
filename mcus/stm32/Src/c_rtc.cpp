/*
 * c_rtc.cpp
 *
 *  Created on: Jun 1, 2021
 *      Author: xrj
 */


#include "c_rtc.hpp"
#ifdef STM32_INC_C_RTC_HPP_
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
  while ((Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET){
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
  while ((Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
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

	high1 = READ_REG(Instance->CNTH & RTC_CNTH_RTC_CNT);
	low   = READ_REG(Instance->CNTL & RTC_CNTL_RTC_CNT);
	high2 = READ_REG(Instance->CNTH & RTC_CNTH_RTC_CNT);

	if(high1 != high2){
	  low = READ_REG(Instance->CNTL & RTC_CNTL_RTC_CNT);
	}
	return (((u32)high2 << 16U) | low);
}

X_State C_RTC::set_cnt(uint32_t TimeCounter)
{
	WRITE_REG(Instance->CNTH, (TimeCounter >> 16U));
	WRITE_REG(Instance->CNTL, (TimeCounter & RTC_CNTL_RTC_CNT));
	if(Instance->CRL & RTC_CRL_RTOFF){
		return X_OK;
	}else{
		return X_Error;
	}
}

uint32_t C_RTC::get_alarm_cnt()
{
	  uint16_t high1 = 0U, low = 0U;

	  high1 = READ_REG(Instance->ALRH & RTC_CNTH_RTC_CNT);
	  low   = READ_REG(Instance->ALRL & RTC_CNTL_RTC_CNT);

	  return (((uint32_t) high1 << 16U) | low);
}

X_State C_RTC::set_alarm_cnt(uint32_t AlarmCounter, bool IT)
{
	X_State status = X_OK;
	if(EnterInitMode() != X_OK){
		status = X_Error;
	}else{
		WRITE_REG(Instance->ALRH, (AlarmCounter >> 16U));
		WRITE_REG(Instance->ALRL, (AlarmCounter & RTC_ALRL_RTC_ALR));

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
	return READ_REG(Instance->DIVL);
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

void C_RTC::set_clock(u32 prescale, u8 calib)
{
	Init.AsynchPrediv = prescale;
	HAL_RTC_Init(this);
	HAL_RTCEx_SetSmoothCalib(this, 0, 0, calib);
}

void C_RTC::set_calib(u8 calib)
{
	HAL_RTCEx_SetSmoothCalib(this, 0, 0, calib);
}

u8 C_RTC::get_calib()
{
	return BKP->RTCCR & 0x3f;
}

u32 C_RTC::get_prescale()
{
	u32 psc;
	while(Instance->DIVL || Instance->DIVH); //wait to all are 0
	while(!psc){
		psc  = Instance->DIVH << 16;
		psc |= Instance->DIVL;
	}
	return psc;
}

#ifdef USE_USB
extern USBD_HandleTypeDef hUsbDeviceFS;
extern C_RTC *rtc;
u16 usb_count=0;
u16 usb_ms;
u8 usb_buff[6];
int8_t usb_callback(u8 event_idx, u8 state);

void C_RTC::USB_Calib(USBD_HandleTypeDef *usb)
{
	usb_count = 0;
	memset(usb_buff, 0, 6);
	USBD_CUSTOM_HID_ItfTypeDef *itf = (USBD_CUSTOM_HID_ItfTypeDef*)hUsbDeviceFS.pUserData;
	itf->OutEvent = &usb_callback;
	while(usb_buff[0] != 0xff){
		if(usb_count){
			usb_count--;
			*(u32*)&usb_buff[2] = rtc->get_cnt();
			*(u16*)&usb_buff[0] = rtc->get_divl();
			USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, usb_buff, 6);
		}
		XDelayMs(usb_ms);
	}
}

int8_t usb_callback(u8 event_idx, u8 state)
{
	USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef *)hUsbDeviceFS.pClassData;
	switch(event_idx){
	case 0x00:  //返回收到数据包的时间
		*(u16*)&usb_buff[0] = rtc->get_divl();
		*(u32*)&usb_buff[2] = rtc->get_cnt();
		USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, usb_buff, 6);
		break;
	case 0x01:  //连续发送时间戳
		usb_count = *(u16*)(hhid->Report_buf+1);
		usb_ms = *(u16*)(hhid->Report_buf+3);
		break;
	case 0x02:  //停止连续发送时间戳
		usb_count = 0;
		break;
	case 0x03:  //读取校准
		usb_buff[0] = rtc->get_calib();
		USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, usb_buff, 1);
		break;
	case 0x04:  //设置校准
		rtc->set_calib(hhid->Report_buf[1]);
		break;
	case 0x05:  //设置分频和校准
		rtc->set_clock(*(u32*)(hhid->Report_buf+1), hhid->Report_buf[5]);
		break;
	case 0x06:  //准备CNT
		*(u32*)usb_buff = *(u32*)(hhid->Report_buf+1);
		rtc->EnterInitMode();
		break;
	case 0x07:  //设置已准备的CNT值
		rtc->set_cnt(*(u32*)usb_buff);
		rtc->ExitInitMode();
		break;
	case 0x08:  //退出
		memset(usb_buff, -1, 6);
		rtc->ExitInitMode();
		break;
	default:
		break;
	}
}
#endif
#endif
