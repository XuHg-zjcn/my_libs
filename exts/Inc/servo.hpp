/*
 * servo.hpp
 *
 *  Created on: 2021年6月30日
 *      Author: xrj
 */

#include "c_pwm.hpp"
#if !defined(__SERVO_HPP__) && \
	defined(__C_PWM_HPP__)
#define __SERVO_HPP__


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

#endif /* __SERVO_HPP__ */
