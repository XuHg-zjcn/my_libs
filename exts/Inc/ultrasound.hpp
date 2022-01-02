/*
 * hc_sr04.h
 *
 *  Created on: 2021年4月16日
 *      Author: xrj
 */

#include "c_tim.hpp"
#if !defined(__ULTRASOUND_HPP__) && \
	defined(__C_TIM_HPP__)
#define __ULTRASOUND_HPP__

#include <stdbool.h>
#include "mylibs_config.hpp"

typedef struct{
	uint32_t *Trig_ODR_bit;
	uint32_t *Echo_IDR_bit;
}US_Connect;

typedef enum{
	US_Mode_While1 = 0,
	US_Mode_EXTI,
	US_Mode_TIM,
	US_Mode_Analog
}US_Mode;

typedef struct{
	ADC_HandleTypeDef* hadc;
	uint32_t* pData;
	uint32_t Length;
}US_ADCDMA;

typedef struct{
	C_TIM *ctim;  // Trig and Echo must in same TIM.
	TIM_CHx Trig_Channel;
	TIM_CHx Echo_Channel_Rising;
	TIM_CHx Echo_Channel_Falling;
	uint32_t freq;
	uint32_t tTrig;
	uint32_t tmax;
	uint32_t t1;
	uint32_t t2;
}US_Timer12;

typedef struct{   // defalut unit
	float temp;   //    25.0 oC
	float hum;    //     0.5
	float press;  //  101325 Pa
	float speed;
	float xm;
}US_Correct;

class UltSnd{
private:
	US_Connect conn;
	US_Mode mode;
	US_ADCDMA adc;
	US_Timer12 timer;
	US_Correct corr;
	float last_m;
	float error;
public:
	UltSnd();
	void Init(US_Connect &conn, US_Timer12 &timer);
	void Measure_While1();
	void Measure_TIM(_Bool blocking);
	float CorrectSpeed();
	float meter();
	float Measure_calc();
	void TIM_CaptureCallback(TIM_CHx Channel);
	void TIM_PeriodElapsedCallback();
};

#endif /* __ULTRASOUND_HPP__ */
