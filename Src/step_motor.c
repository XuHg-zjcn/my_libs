/*
 * step_motor.c
 *
 *  Created on: Apr 15, 2021
 *      Author: xrj
 */

#include "step_motor.h"
#include <math.h>
#include "stdlib.h"
#include "my_gpio.h"
#include "timer.h"

extern TIM_HandleTypeDef htim2;
#define SM_TIMx htim2

const StepMotor_State seq_41[4] = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};  //单四拍
const StepMotor_State seq_42[4] = {{1,1,0,0}, {0,1,1,0}, {0,0,1,1}, {1,0,0,1}};  //双四拍
const StepMotor_State seq_8[8]  = {{1,0,0,0}, {1,1,0,0}, {0,1,0,0}, {0,1,1,0},   //八拍
		                       {0,0,1,0}, {0,0,1,1}, {0,0,0,1}, {1,0,0,1}};
const StepMotor_State stop = {0,0,0,0};

const StepMotor_State* seqs[3] = {seq_41, seq_42, seq_8};
const uint32_t seqs_len[3] = {4, 4, 8};

/*
 * load default value to StepMotor_Handle.
 * hsm->conn need assign by user
 */
void StepMotor_LoadDefault(StepMotor_Handle *hsm)
{
	hsm->seq = seq_8;
	hsm->seq_len = 8;
	hsm->total_rot = 0;
	hsm->remain_step = -1;
	hsm->rot = 0;
	hsm->FinishCallback = DefaultFinishCallback;
}

/*
 * if you don't assign all value, please call `StepMotor_LoadDefault`,
 * to avoid error happend.
 */
void StepMotor_Init(StepMotor_Handle *hsm)
{
	//stop TIM
	HAL_TIM_Base_Stop_IT(hsm->conn->htimx);

	//init GPIO Pins
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	uint32_t **pin = &hsm->conn->P1;
	for(int i=0;i<4;i++){
		**pin = 0;
		GPIO_InitStruct.Pin = BITBAND_GPIO_PIN_2N(*pin);
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	    HAL_GPIO_Init(BITBAND_GPIO_GPIOx(*pin), &GPIO_InitStruct);
	    pin++;
	}
}

void StepMotor_setState(StepMotor_Handle *hsm, StepMotor_State State)
{
	uint32_t **pin = &hsm->conn->P1;
	uint32_t *bx = BIT_PTR(&State, 0);
	for(int i=0;i<4;i++){
		**pin = *bx;
		pin++;
		bx++;
	}
}

void StepMotor_setMode(StepMotor_Handle *hsm, StepMotor_Mode mode)
{
	hsm->seq = seqs[mode];
	uint8_t new_len = seqs_len[mode];
	if(new_len > hsm->seq_len){
		hsm->seq_i *= 2;
	}else if(new_len < hsm->seq_len){
		hsm->seq_i /= 2;
	}
	hsm->seq_len = new_len;
}

void StepMotor_Stop(StepMotor_Handle *hsm){
	hsm->rot = 0;
	hsm->seq_i = 0;
	hsm->remain_step = 0;
	HAL_TIM_Base_Stop_IT(hsm->conn->htimx);
	StepMotor_setState(hsm, stop);
	hsm->FinishCallback(hsm);
}

void StepMotor_set_us(StepMotor_Handle *hsm, int us, uint32_t steps)
{
	if(us == 0){
		StepMotor_Stop(hsm);
		return;
	}
	hsm->rot = 0;
	hsm->rot += (us>0);
	hsm->rot -= (us<0);
	if(steps <= 0){
		hsm->remain_step = -1;
	}
	hsm->remain_step = steps;
	uint32_t AutoReload = ((uint64_t)GetTimerFreq(&SM_TIMx)*abs(us))/1000000;
	__HAL_TIM_SET_AUTORELOAD(hsm->conn->htimx, AutoReload-1);
	HAL_TIM_Base_Start_IT(hsm->conn->htimx);
}

void StepMotor_setSpeed(StepMotor_Handle *hsm, float deg_sec, float total_deg)
{
	if(deg_sec == 0){
		StepMotor_Stop(hsm);
		return;
	}
	hsm->rot = 0;
	hsm->rot += (deg_sec>0);
	hsm->rot -= (deg_sec<0);
	float step_sec = deg_sec*(512.0/360.0)*hsm->seq_len;
	if(total_deg >= 0){
		hsm->remain_step = total_deg*(512.0/360.0)*hsm->seq_len;
	}else{
		hsm->remain_step = -1;
	}
	uint32_t AutoReload = (float)GetTimerFreq(&SM_TIMx)/fabs(step_sec);
	__HAL_TIM_SET_AUTORELOAD(&SM_TIMx, AutoReload-1);
	HAL_TIM_Base_Start_IT(&SM_TIMx);
}

void StepMotor_RunStep(StepMotor_Handle *hsm)
{
	if(hsm->rot == 0){
		return;
	}
	StepMotor_setState(hsm, hsm->seq[hsm->seq_i]);
	hsm->seq_i += hsm->rot;
	hsm->total_rot += hsm->rot;
	if(hsm->remain_step>0){
		hsm->remain_step--;
	}if(hsm->remain_step==0){
		StepMotor_Stop(hsm);
		return;
	}
	if(hsm->seq_i >= hsm->seq_len){
		hsm->seq_i = 0;
	}if(hsm->seq_i < 0){
		hsm->seq_i = hsm->seq_len-1;
	}
}
