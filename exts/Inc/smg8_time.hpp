/*
 * smg_time.hpp
 *
 *  Created on: Jul 1, 2021
 *      Author: xrj
 */

#ifndef EXTS_INC_SMG8_TIME_HPP_
#define EXTS_INC_SMG8_TIME_HPP_

#include "smg8.hpp"

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



#endif /* EXTS_INC_SMG8_TIME_HPP_ */
