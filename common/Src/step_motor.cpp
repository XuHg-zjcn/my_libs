/*
 * step_motor.c
 *
 *  Created on: Apr 15, 2021
 *      Author: xrj
 */

#include "step_motor.hpp"

#include <math.h>
#include <stdlib.h>
#include "mylibs_config.hpp"
#include "bit_band.h"
#include "c_tim.hpp"

extern TIM_HandleTypeDef htim2;
extern RTC_HandleTypeDef hrtc;
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
StepMotor::StepMotor(GPIO_Conn &conn, C_TIM *htimx)
{
	seq = seq_8;
	seq_len = 8;
	total_rot = 0;
	remain_step = -1;
	rot = 0;
	FinishCallback = DefaultFinishCallback;
	timeout = 1000;
	this->htimx = htimx;
	conn.Enable();
	for(int i=0;i<4;i++){
		odr_bitband[i] = conn[i]->p8b.ODR_bitband();
	}
	rot_state = (int16_t)HAL_RTCEx_BKUPRead(&hrtc, ROT_STATE_BKPREG_NUM);
}

/*
 * if you don't assign all value, please call `StepMotor_LoadDefault`,
 * to avoid error happend.
 */
void StepMotor::Init()
{
	//stop TIM
	HAL_TIM_Base_Stop_IT(htimx);

	//init GPIO Pins
	osSemaphoreAttr_t attr_sem = {.name = "step_motor_sem"};
	sem = osSemaphoreNew(1, 1, &attr_sem);
}

void StepMotor::setState(StepMotor_State State)
{
	uint32_t **odr = odr_bitband;
	uint32_t *bx = BIT_PTR(&State, 0);
	for(int i=0;i<4;i++){
		**odr = *bx;
		odr++;
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
	HAL_TIM_Base_Stop_IT(htimx);
	setState(stop);
	osSemaphoreRelease(sem);
	FinishCallback(this);
}

void StepMotor::wait()
{
	osSemaphoreAcquire(sem, timeout);
	osSemaphoreRelease(sem);
}

void StepMotor::run_us(uint32_t us, int32_t steps, bool blocking)
{
	osSemaphoreAcquire(sem, timeout);
	if(steps == 0){
		Stop();
		return;
	}
	rot = steps>0?1:-1;
	remain_step = abs(steps);
	htimx->set_ns((uint64_t)us*1000UL);
	HAL_TIM_Base_Start_IT(htimx);
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
	htimx->set_Hz(step_sec);
	HAL_TIM_Base_Start_IT(htimx);
	if(blocking){
		wait();
	}
}

//please call in timer update interrupt
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
	rot_state += seq_len==8 ? rot : rot*2;
	HAL_RTCEx_BKUPWrite(&hrtc, ROT_STATE_BKPREG_NUM, rot_state);
}

int16_t StepMotor::get_rot_state()
{
	return rot_state;
}
