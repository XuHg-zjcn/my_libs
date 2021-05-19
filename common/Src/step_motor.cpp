/*
 * step_motor.c
 *
 *  Created on: Apr 15, 2021
 *      Author: xrj
 */

#include "step_motor.hpp"

#include <math.h>
#include <stdlib.h>
#include "c_gpio.h"
#include "c_tim.hpp"

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
StepMotor::StepMotor()
{
	seq = seq_8;
	seq_len = 8;
	total_rot = 0;
	remain_step = -1;
	rot = 0;
	FinishCallback = DefaultFinishCallback;
	timeout = 1000;
}

/*
 * if you don't assign all value, please call `StepMotor_LoadDefault`,
 * to avoid error happend.
 */
void StepMotor::Init(StepMotor_Connect &conn)
{
	this->conn = conn;
	//stop TIM
	HAL_TIM_Base_Stop_IT(this->conn.htimx);

	//init GPIO Pins
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	uint32_t **pin = &this->conn.P1;
	for(int i=0;i<4;i++){
		**pin = 0;
		GPIO_InitStruct.Pin = BITBAND_GPIO_PIN_2N(*pin);
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	    HAL_GPIO_Init(BITBAND_GPIO_GPIOx(*pin), &GPIO_InitStruct);
	    pin++;
	}
	osSemaphoreAttr_t attr_sem = {.name = "step_motor_sem"};
	sem = osSemaphoreNew(1, 1, &attr_sem);
}

void StepMotor::setState(StepMotor_State State)
{
	uint32_t **pin = &conn.P1;
	uint32_t *bx = BIT_PTR(&State, 0);
	for(int i=0;i<4;i++){
		**pin = *bx;
		pin++;
		bx++;
	}
}

void StepMotor::setMode(StepMotor_Mode mode)
{
	seq = seqs[mode];
	uint8_t new_len = seqs_len[mode];
	if(new_len > seq_len){
		seq_i *= 2;
	}else if(new_len < seq_len){
		seq_i /= 2;
	}
	seq_len = new_len;
}

void StepMotor::Stop()
{
	rot = 0;
	seq_i = 0;
	remain_step = 0;
	HAL_TIM_Base_Stop_IT(conn.htimx);
	setState(stop);
	osSemaphoreRelease(sem);
	FinishCallback(this);
}

void StepMotor::wait()
{
	osSemaphoreAcquire(sem, timeout);
	osSemaphoreRelease(sem);
}

//TODO: uint32_t us, int32_t step
void StepMotor::run_us(int us, uint32_t steps, bool blocking)
{
	osSemaphoreAcquire(sem, timeout);
	if(us == 0){
		Stop();
		return;
	}
	rot = 0;
	rot += (us>0);
	rot -= (us<0);
	remain_step = steps<=0 ? -1 : steps;
	conn.htimx->set_ns((uint64_t)us*1000UL);
	HAL_TIM_Base_Start_IT(conn.htimx);
	if(blocking){
		wait();
	}
}

void StepMotor::run_speed(float deg_sec, float total_deg, bool blocking)
{
	osSemaphoreAcquire(sem, timeout);
	if(deg_sec == 0){
		Stop();
		return;
	}
	rot = 0;
	rot += (deg_sec>0);
	rot -= (deg_sec<0);
	float step_sec = deg_sec*(512.0/360.0)*seq_len;
	if(total_deg >= 0){
		remain_step = total_deg*(512.0/360.0)*seq_len;
	}else{
		remain_step = -1;
	}
	conn.htimx->set_Hz(step_sec);
	HAL_TIM_Base_Start_IT(conn.htimx);
	if(blocking){
		wait();
	}
}

void StepMotor::run_step()
{
	if(rot == 0){
		return;
	}
	setState(seq[seq_i]);
	seq_i += rot;
	total_rot += rot;
	if(remain_step>0){
		remain_step--;
	}if(remain_step==0){
		Stop();
		return;
	}
	if(seq_i >= seq_len){
		seq_i = 0;
	}if(seq_i < 0){
		seq_i = seq_len-1;
	}
}
