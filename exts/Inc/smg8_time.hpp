/*
 * smg_time.hpp
 *
 *  Created on: Jul 1, 2021
 *      Author: xrj
 */

#include "smg8.hpp"
#include "c_rtc.hpp"
#if !defined(__SMG8_TIME_HPP__) && \
	defined(__SMG8_HPP__) && \
	defined(__C_RTC_HPP__)
#define __SMG8_TIME_HPP__


class SMG_Time : public SMG8{
private:
	C_RTC* rtc;
	PinState colon_state;  //need by PWM light
public:
	void connRTC(C_RTC* rtc);
	void showTime(int n, PinState c, int m);
	void showCurrYear();
	void showCurrDate();
	void startTimeDymtic();
	void stopTimeDymitc();
	void RTCSecondCallback();
};

#endif /* __SMG8_TIME_HPP__ */
