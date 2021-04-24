/*
 * hc_sr04.h
 *
 *  Created on: 2021年4月16日
 *      Author: xrj
 */

#ifndef INC_ULTRASOUND_H_
#define INC_ULTRASOUND_H_


#include "stm32f4xx_hal.h"

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
	TIM_HandleTypeDef *htimx;  // Trig and Echo must in same TIM.
	uint32_t Trig_Channel;
	uint32_t Echo_Channel_Rising;
	uint32_t Echo_Channel_Falling;
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

typedef struct{
	US_Connect conn;
	US_Mode mode;
	US_ADCDMA adc;
	US_Timer12 timer;
	US_Correct corr;
	float last_m;
	float error;
}US_Handle;

void US_LoadDefault(US_Handle *hus);
void US_Init(US_Handle *hus);
void US_Measure_TIM(US_Handle *hus, _Bool blocking);
float US_CorrectSpeed(US_Handle *hus);
float US_Measure_calc(US_Handle *hus);
void US_TIM_CaptureCallback(US_Handle *hus, uint32_t Channel);
void US_TIM_PeriodElapsedCallback(US_Handle *hus);

#endif /* INC_ULTRASOUND_H_ */
