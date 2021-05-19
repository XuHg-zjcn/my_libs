/*
 * hc_sr04.c
 *
 *  Created on: 2021年4月16日
 *      Author: xrj
 */

#include "ultrasound.hpp"

#include <math.h>
#include "delay.h"
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
	this->timer.freq = this->timer.htimx->get_Hz(TIM_ClkLv_PSC16b);
	this->timer.tmax = this->timer.freq/10;        // 触发周期100ms
	this->timer.tTrig = this->timer.freq/10000;    // 触发高电平100us
}

void UltSnd::Measure_While1()
{
	*(this->conn.Trig_ODR_bit) = 1;
	Delay_us(100);
	*(this->conn.Trig_ODR_bit) = 0;

	this->timer.t1 = 0;
	this->timer.t2 = 0;
	while(!*(this->conn.Echo_IDR_bit)){
		this->timer.t1++;
	}
	while(!*(this->conn.Echo_IDR_bit)){
		this->timer.t2++;
	}
	this->timer.t2 += this->timer.t1;
}

void UltSnd::Measure_TIM(_Bool blocking)
{
	timer.t1 = 0;
	timer.t2 = 0;
	__HAL_TIM_DISABLE(this->timer.htimx);

	timer.htimx->Instance->CR1 |= TIM_CR1_OPM;
	//OPM, CNT will 0 after UPDATE, so use PWM2
	timer.htimx->set_OCMode(timer.Trig_Channel, TIM_OCMode_PWM2);

	__HAL_TIM_SET_COUNTER(timer.htimx, 0);
	__HAL_TIM_SET_AUTORELOAD(timer.htimx, timer.tmax);
	__HAL_TIM_SET_COMPARE(timer.htimx, timer.Trig_Channel, timer.tmax-timer.tTrig);

	__HAL_TIM_ENABLE_IT(timer.htimx, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(timer.htimx, timer.Echo_Channel_Rising);
	__HAL_TIM_ENABLE_IT(timer.htimx, timer.Echo_Channel_Falling);

	TIM_CCxChannelCmd(timer.htimx->Instance, timer.Trig_Channel, TIM_CCx_ENABLE);
	TIM_CCxChannelCmd(timer.htimx->Instance, timer.Echo_Channel_Rising, TIM_CCx_ENABLE);
	TIM_CCxChannelCmd(timer.htimx->Instance, timer.Echo_Channel_Falling, TIM_CCx_ENABLE);
	__HAL_TIM_ENABLE(timer.htimx);
	if(blocking){
		while(!this->timer.t2);
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
    T = this->corr.temp + 273.15;
    this->corr.speed = sqrt(1.40*8.314*T/M);
	return this->corr.speed;
}

float UltSnd::meter()
{
	uint32_t count = this->timer.t2 - this->timer.t1;
	float sec = (float)count / this->timer.freq;
	return this->corr.speed/2*sec;
}

float UltSnd::Measure_calc()
{
	switch(this->mode){
	case US_Mode_While1:
		this->Measure_While1();
		break;
	case US_Mode_EXTI:
		break;
	case US_Mode_TIM:
		this->Measure_TIM(true);
		break;
	case US_Mode_Analog:
		HAL_ADC_Start_DMA(this->adc.hadc, this->adc.pData, this->adc.Length);
		this->Measure_TIM(false);
	}
	return this->meter();
}

void UltSnd::TIM_CaptureCallback(uint32_t Channel)
{
	if(Channel == this->timer.Echo_Channel_Rising){
		__HAL_TIM_DISABLE_IT(this->timer.htimx, Channel);
		this->timer.t1 = HAL_TIM_ReadCapturedValue(this->timer.htimx, Channel);
	}
	else if(Channel == this->timer.Echo_Channel_Falling){
		__HAL_TIM_DISABLE_IT(this->timer.htimx, Channel);
		this->timer.t2 = HAL_TIM_ReadCapturedValue(this->timer.htimx, Channel);
	}
}

void UltSnd::TIM_PeriodElapsedCallback()
{
	if(this->timer.htimx->Instance->CR1 & TIM_CR1_OPM){
		this->timer.htimx->set_OCMode(this->timer.Trig_Channel, TIM_OCMode_Forced_InActive);
		this->timer.htimx->Instance->CR1 &= (~TIM_CR1_OPM);
	}
}