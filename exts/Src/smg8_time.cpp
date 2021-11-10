/*
 * smg8_time.cpp
 *
 *  Created on: Jul 1, 2021
 *      Author: xrj
 */

#include "smg8_time.hpp"

#ifdef EXTS_INC_SMG8_TIME_HPP_
void SMG_Time::connRTC(C_RTC* rtc)
{
	this->rtc = rtc;
}

void SMG_Time::showTime(int n, PinState c, int m)
{
	showNum(0, 2, n, 0, false);
	showNum(2, 4, m, 0, true);
	colon_state = c;
}

void SMG_Time::showCurrYear()
{
	showNum(rtc->get_tm()->tm_year+1900);
}

void SMG_Time::showCurrDate()
{
	struct tm *tm = rtc->get_tm();
	showNum(0, 2, tm->tm_mon+1, 2, false);
	showNum(2, 4, tm->tm_mday, 0, false);
}

/*
 * need call `RTCSecondCallback` in `HAL_RTCEx_RTCEventCallback`
 */
void SMG_Time::startTimeDymtic()
{
	rtc->EnableIT(RTC_IT_SEC);
}

void SMG_Time::stopTimeDymitc()
{
	rtc->DisableIT(RTC_IT_SEC);
}

/*
 * callback of RTC second interrupt.
 * note: NVIC RTC priority should lower than TIM.
 */
void SMG_Time::RTCSecondCallback()
{
	struct tm *tm2 = rtc->get_tm();
	this->showTime(tm2->tm_hour, tm2->tm_sec%2==0?Pin_Set:Pin_Reset, tm2->tm_min);
}
#endif
