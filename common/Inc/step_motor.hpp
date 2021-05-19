/*
 * step_motor.h
 *
 *  Created on: Apr 15, 2021
 *      Author: xrj
 */

#ifndef INC_STEP_MOTOR_HPP_
#define INC_STEP_MOTOR_HPP_

#include "main.h"
#include "c_tim.hpp"
#include "cmsis_os2.h"

typedef struct{
	unsigned int b1:1;
	unsigned int b2:1;
	unsigned int b3:1;
	unsigned int b4:1;
}StepMotor_State;

typedef enum{
	StepMotor_Single4 = 0,
	StepMotor_Double4,
	StepMotor_8
}StepMotor_Mode;

typedef struct{
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
}GPIO_Pin;

//don't need channel, only use PeriodElapsedCallback.
typedef struct{
	uint32_t *P1;
	uint32_t *P2;
	uint32_t *P3;
	uint32_t *P4;
	C_TIM *htimx;
}StepMotor_Connect;

class StepMotor{
private:
	StepMotor_Connect conn;
	const StepMotor_State *seq;
	int32_t seq_len;
	int32_t seq_i;        // 0 <= seq_i < seq_len
	int64_t total_rot;    // +=rot; each RunStep
	int32_t remain_step;  // --; each RunStep, -1 will always run;
	int32_t rot;          // 1:CCW, -1:CW, 0:stop
	osSemaphoreId_t sem;
	uint32_t timeout;
	void setState(StepMotor_State State);
public:
	void (*FinishCallback)(void*);  // callback of `stop`
	StepMotor();
	void Init(StepMotor_Connect &conn);
	void setMode(StepMotor_Mode mode);
	void Stop();
	void wait();
	void run_us(int us, uint32_t steps, bool blocking);
	void run_speed(float deg_sec, float total_deg, bool blocking);
	void run_step();
};


__weak void DefaultFinishCallback(void *hsmx)
{
	UNUSED(hsmx);
}


#endif /* INC_STEP_MOTOR_HPP_ */
