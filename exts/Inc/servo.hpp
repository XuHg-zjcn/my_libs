/*
 * servo.hpp
 *
 *  Created on: 2021年6月30日
 *      Author: xrj
 */

#include "c_pwm.hpp"
#if !defined(EXTS_INC_SERVO_HPP_) && defined(STM32_INC_C_PWM_HPP_)
#define EXTS_INC_SERVO_HPP_


class Servo{
private:
	C_PWM_CH *ch;
	float cur_deg;
public:
	Servo(C_PWM_CH *ch);
	void Init();
	void set_deg(float deg);
	void slow_run(float deg, float deg_sec);
};


#endif /* EXTS_INC_SERVO_HPP_ */
