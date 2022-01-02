/*
 * smg_pins.hpp
 *
 *  Created on: Jul 1, 2021
 *      Author: xrj
 */

#include "smg8.hpp"
#include "c_tim.hpp"
#if !defined(__SMG8_PINS_HPP__) && \
	defined(__SMG8_HPP__) && \
	defined(__C_TIM_HPP__)
#define __SMG8_PINS_HPP__

class SMG8_Pins{
private:
	GPIO_Conn *seg;
	GPIO_Conn *xb;
	C_Pin *colon;
	uint8_t *buf;
	PinState colon_state;
	int index;  //modify in `TimerFunc()`
#ifdef INC_STM32_TIM_HPP_
	C_TIM* ctim;
#endif
public:
	SMG8_Pins();
	void LightMode();
	void DarkMode();
	void off();
#ifdef INC_STM32_TIM_HPP_
	void connTIM(C_TIM* ctim);
#endif
	void TimerFunc();
#ifdef USE_FREERTOS
	void FreeRTOSTimer(uint32_t ms_on, uint32_t ms_off);
#endif
#ifdef INC_STM32_TIM_HPP_
	void HardWareTimer(uint32_t us_on, uint32_t us_off);
#endif
};

#endif /* __SMG8_PINS_HPP__ */
