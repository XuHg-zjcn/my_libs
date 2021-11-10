/*
 * servo.cpp
 *
 *  Created on: 2021年6月30日
 *      Author: xrj
 */

#include "servo.hpp"
#ifdef EXTS_INC_SERVO_HPP_
Servo::Servo(C_PWM_CH *ch)
{
	this->ch = ch;
	this->cur_deg = 0;
}

void Servo::Init()
{
	ch->set_ns(20000000);
	ch->start();
}

void Servo::set_deg(float deg)
{
	ch->set_duty(0.025f + deg*(0.1f/180.0f));
}

void Servo::slow_run(float deg, float deg_sec)
{
	float deg_step = deg_sec*0.02f;
	if(deg < cur_deg){
		deg_step *= -1;
	}
	int steps = (deg - cur_deg)/deg_step;
	for(;steps>0;steps--){
		cur_deg += deg_step;
		set_deg(cur_deg);
		XDelayMs(20);
	}
}
#endif
