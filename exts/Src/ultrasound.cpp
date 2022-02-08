/*
 * hc_sr04.c
 *
 *  Created on: 2021年4月16日
 *      Author: xrj
 */

#include "ultrasound.hpp"
#ifdef __ULTRASOUND_H__

#include <math.h>
#include "delay.hpp"
#include "c_tim.hpp"

UltSnd::UltSnd()
{
	this->corr.temp = 25.0;
	this->corr.hum = 0.5;
	this->corr.press = 101325.0;
	this->CorrectSpeed();
	//TIM of ultrasound Trig
}

void UltSnd::Init(US_Connect &conn, US_Timer12 &timer)
{
	this->conn = conn;
	this->timer = timer;
	this->timer.freq = timer.ctim->get_Hz(TIM_ClkLv_PSC16b);
	this->timer.tmax = timer.freq/10;        // 触发周期100ms
	this->timer.tTrig = timer.freq/10000;    // 触发高电平100us
}

void UltSnd::Measure_While1()
{
	*(conn.Trig_ODR_bit) = 1;
	Delay_us(100);
	*(conn.Trig_ODR_bit) = 0;

	timer.t1 = 0;
	timer.t2 = 0;
	while(!*(conn.Echo_IDR_bit)){
		this->timer.t1++;
	}
	while(!*(conn.Echo_IDR_bit)){
		this->timer.t2++;
	}
	timer.t2 += timer.t1;
}

void UltSnd::Measure_TIM(_Bool blocking)
{
	timer.t1 = 0;
	timer.t2 = 0;
	TIM_HandleTypeDef *htim = timer.ctim->htim;
	__HAL_TIM_DISABLE(htim);

	//TODO: use one pulse mode in C_TIM
	htim->Instance->CR1 |= TIM_CR1_OPM;
	//OPM, CNT will 0 after UPDATE, so use PWM2
	timer.ctim->set_OCMode(timer.Trig_Channel, TIM_OCMode_PWM2);

	//TODO: remove STM32 HAL functions
	__HAL_TIM_SET_COUNTER(htim, 0);
	__HAL_TIM_SET_AUTORELOAD(htim, timer.tmax);
	__HAL_TIM_SET_COMPARE(htim, timer.Trig_Channel, timer.tmax-timer.tTrig);

	__HAL_TIM_ENABLE_IT(htim, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(htim, timer.Echo_Channel_Rising);
	__HAL_TIM_ENABLE_IT(htim, timer.Echo_Channel_Falling);

	TIM_CCxChannelCmd(htim->Instance, timer.Trig_Channel, TIM_CCx_ENABLE);
	TIM_CCxChannelCmd(htim->Instance, timer.Echo_Channel_Rising, TIM_CCx_ENABLE);
	TIM_CCxChannelCmd(htim->Instance, timer.Echo_Channel_Falling, TIM_CCx_ENABLE);
	__HAL_TIM_ENABLE(htim);
	if(blocking){
		while(!timer.t2);
	}
}

float UltSnd::CorrectSpeed()
{
	//All value are SI units.
	float p_w;  // press of water
	float M, T;
	//Antoine equation lgP = A - B/(t+C), saturated vapor pressure
	p_w = pow(10, 10.07406 - 1657.46/(this->corr.temp+227.02));
	p_w *= this->corr.hum;              // current vapor pressure
	//M = M_w*p_w/press + M_dry*(press-p_w)/press = p_w/press*(M_w-M_dry) + M_dry
	M = (0.01802-0.02896)*p_w / this->corr.press + 0.02896;
    //v = sqrt(γ*p/ρ) = sqrt(γ*R*T/M)
    T = corr.temp + 273.15;
    corr.speed = sqrt(1.40*8.314*T/M);
	return corr.speed;
}

float UltSnd::meter()
{
	uint32_t count = timer.t2 - timer.t1;
	float sec = (float)count / timer.freq;
	return corr.speed/2*sec;
}

float UltSnd::Measure_calc()
{
	switch(mode){
	case US_Mode_While1:
		Measure_While1();
		break;
	case US_Mode_EXTI:
		break;
	case US_Mode_TIM:
		Measure_TIM(true);
		break;
	case US_Mode_Analog:
		HAL_ADC_Start_DMA(adc.hadc, adc.pData, adc.Length);
		Measure_TIM(false);
	}
	return this->meter();
}

void UltSnd::TIM_CaptureCallback(TIM_CHx Channel)
{
	if(Channel == timer.Echo_Channel_Rising){
		timer.ctim->DisableIT(TIM_CH2IT(Channel));
		timer.t1 = timer.ctim->ReadCapturedValue(Channel);
	}
	else if(Channel == timer.Echo_Channel_Falling){
		timer.ctim->DisableIT(TIM_CH2IT(Channel));
		timer.t2 = timer.ctim->ReadCapturedValue(Channel);
	}
}

void UltSnd::TIM_PeriodElapsedCallback()
{
	if(timer.ctim->htim->Instance->CR1 & TIM_CR1_OPM){
		timer.ctim->set_OCMode(timer.Trig_Channel, TIM_OCMode_Forced_InActive);
		timer.ctim->htim->Instance->CR1 &= (~TIM_CR1_OPM);
	}
}

#endif
