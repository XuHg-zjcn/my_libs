/*
 * smg_pins.hpp
 *
 *  Created on: Jul 1, 2021
 *      Author: xrj
 */

#ifndef EXTS_INC_SMG8_PINS_HPP_
#define EXTS_INC_SMG8_PINS_HPP_

#include "smg8.hpp"

class SMG8_Pins{
private:
	GPIO_Conn *seg;
	GPIO_Conn *xb;
	C_Pin *colon;
	uint8_t *buf;
	PinState colon_state;
	int index;  //modify in `TimerFunc()`
	C_TIM* ctim;
public:
	SMG8_Pins();
	void LightMode();
	void DarkMode();
	void off();
	void connTIM(C_TIM* ctim);
	void TimerFunc();
#ifdef USE_FREERTOS
	void FreeRTOSTimer(uint32_t ms_on, uint32_t ms_off);
#endif
	void HardWareTimer(uint32_t us_on, uint32_t us_off);
};



#endif /* EXTS_INC_SMG8_PINS_HPP_ */
