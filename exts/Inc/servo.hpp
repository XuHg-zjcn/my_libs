/*
 * servo.hpp
 *
 *  Created on: 2021年6月30日
 *      Author: xrj
 */

#ifndef EXTS_INC_SERVO_HPP_
#define EXTS_INC_SERVO_HPP_

#include "c_pwm.hpp"

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
