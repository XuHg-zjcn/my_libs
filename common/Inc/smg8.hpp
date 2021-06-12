/*
 * smg.hpp
 *
 *  Created on: 2021年6月2日
 *      Author: xrj
 */

#ifndef COMMON_INC_SMG8_HPP_
#define COMMON_INC_SMG8_HPP_

#include "mylibs_config.hpp"
#include "c_tim.hpp"
#include "c_rtc.hpp"
#include "pins_manager.hpp"

class SMG8{
private:
	GPIO_Conn *seg;
	GPIO_Conn *xb;
	C_Pin *colon;
	uint8_t buf[4];
	bool colon_state;  //need by PWM light
	int index;  //modify in `TimerFunc()`
	C_RTC* rtc;
	C_TIMEx* etim;
public:
	SMG8();
	void LightMode();
	void DarkMode();
	void off();
	void connRTC(C_RTC* rtc);
	void connTIM(C_TIMEx* etim);
	void showNum(int i0, int i1, int num, int point, bool fill0);
	void showNum(int num1, int point, int num2);
	void showNum(int num, int point) {showNum(0, 4, num, point, false);}
	void showNum(int num)            {showNum(0, 4, num, 0, false);}
	void showNum(float num);
	void showNum(float num, int point);
	void showStr(char* str);
	void showTime(int n, bool c, int m);
	void showCurrYear();
	void showCurrDate();
	void startTimeDymtic();
	void stopTimeDymitc();
	void TimerFunc();
#ifdef USE_FREERTOS
	void FreeRTOSTimer(uint32_t ms_on, uint32_t ms_off);
	void HardWareTimer(uint32_t us_on, uint32_t us_off);
#endif
	void RTCSecondCallback();
};


#endif /* COMMON_INC_SMG_HPP_ */
