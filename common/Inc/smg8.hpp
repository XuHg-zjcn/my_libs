/*
 * smg.hpp
 *
 *  Created on: 2021年6月2日
 *      Author: xrj
 */

#ifndef COMMON_INC_SMG8_HPP_
#define COMMON_INC_SMG8_HPP_

#include "mylibs_config.hpp"
#include "c_rtc.hpp"
#include "pins_manager.hpp"

class SMG8{
private:
	GPIO_Conn *seg;
	GPIO_Conn *xb;
	Pin8b *colon;
	uint8_t buf[4];
	bool colon_state;  //need by PWM light
	int index;  //modify in `TimerFunc()`
	C_RTC* rtc;
public:
	SMG8();
	void LightMode();
	void DarkMode();
	void off();
	void connRTC(C_RTC* rtc);
	void showNum(int num, int point);
	void showStr(char* str);
	void showTime(int n, bool c, int m);
	void startTimeDymtic();
	void stopTimeDymitc();
	void TimerFunc();
#ifdef USE_FREERTOS
	void FreeRTOSTimer(uint32_t ms_on, uint32_t ms_off);
#endif
	//TODO: use hardware timer
	void RTCSecondCallback();
};


#endif /* COMMON_INC_SMG_HPP_ */
