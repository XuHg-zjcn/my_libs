/*
 * hc_sr04.c
 *
 *  Created on: 2021年4月16日
 *      Author: xrj
 */

#include "ultrasound.h"

#include <stdbool.h>
#include <math.h>

#include "delay.h"
#include "timer.h"

void US_LoadDefault(US_Handle *hus)
{
	hus->corr.temp = 25.0;
	hus->corr.hum = 0.5;
	hus->corr.press = 101325.0;
	US_CorrectSpeed(hus);
	hus->timer.freq = GetTimerFreq(hus->timer.htimx);
	hus->timer.tmax = hus->timer.freq/10;
	hus->timer.tTrig = hus->timer.freq/10000;
}

void US_Init(US_Handle *hus)
{
}

void US_Measure_While1(US_Handle *hus)
{
	*(hus->conn.Trig_ODR_bit) = 1;
	Delay_us(100);
	*(hus->conn.Trig_ODR_bit) = 0;

	hus->timer.t1 = 0;
	hus->timer.t2 = 0;
	while(!*(hus->conn.Echo_IDR_bit)){
		hus->timer.t1++;
	}
	while(!*(hus->conn.Echo_IDR_bit)){
		hus->timer.t2++;
	}
	hus->timer.t2 += hus->timer.t1;
}

void US_Measure_TIM(US_Handle *hus, _Bool blocking)
{
	US_Timer12 timer = hus->timer;
	timer.t1 = 0;
	timer.t2 = 0;
	__HAL_TIM_DISABLE(hus->timer.htimx);

	timer.htimx->Instance->CR1 |= TIM_CR1_OPM;
	//OPM, CNT will 0 after UPDATE, so use PWM2
	TIM_OCMode(timer.htimx, timer.Trig_Channel, TIM_OCMODE_PWM2);

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
		while(!hus->timer.t2);
	}
}

float US_CorrectSpeed(US_Handle *hus)
{
	//All value are SI units.
	float p_w;  // press of water
	float M, T;
	//Antoine equation lgP = A - B/(t+C), saturated vapor pressure
	p_w = pow(10, 10.07406 - 1657.46/(hus->corr.temp+227.02));
	p_w *= hus->corr.hum;              // current vapor pressure
	//M = M_w*p_w/press + M_dry*(press-p_w)/press = p_w/press*(M_w-M_dry) + M_dry
	M = (0.01802-0.02896)*p_w / hus->corr.press + 0.02896;
    //v = sqrt(γ*p/ρ) = sqrt(γ*R*T/M)
    T = hus->corr.temp + 273.15;
	hus->corr.speed = sqrt(1.40*8.314*T/M);
	return hus->corr.speed;
}

float US_meter(US_Handle *hus)
{
	uint32_t count = hus->timer.t2 - hus->timer.t1;
	float sec = (float)count / hus->timer.freq;
	return hus->corr.speed/2*sec;
}

float US_Measure_calc(US_Handle *hus)
{
	switch(hus->mode){
	case US_Mode_While1:
		US_Measure_While1(hus);
		break;
	case US_Mode_EXTI:
		break;
	case US_Mode_TIM:
		US_Measure_TIM(hus, true);
		break;
	case US_Mode_Analog:
		HAL_ADC_Start_DMA(hus->adc.hadc, hus->adc.pData, hus->adc.Length);
		US_Measure_TIM(hus, false);
	}
	return US_meter(hus);
}

void US_TIM_CaptureCallback(US_Handle *hus, uint32_t Channel)
{
	if(Channel == hus->timer.Echo_Channel_Rising){
		__HAL_TIM_DISABLE_IT(hus->timer.htimx, Channel);
		hus->timer.t1 = HAL_TIM_ReadCapturedValue(hus->timer.htimx, Channel);
	}
	else if(Channel == hus->timer.Echo_Channel_Falling){
		__HAL_TIM_DISABLE_IT(hus->timer.htimx, Channel);
		hus->timer.t2 = HAL_TIM_ReadCapturedValue(hus->timer.htimx, Channel);
	}
}

void US_TIM_PeriodElapsedCallback(US_Handle *hus)
{
	if(hus->timer.htimx->Instance->CR1 & TIM_CR1_OPM){
		TIM_OCMode(hus->timer.htimx, hus->timer.Trig_Channel, TIM_OCMODE_FORCED_INACTIVE);
		hus->timer.htimx->Instance->CR1 &= (~TIM_CR1_OPM);
	}
}
