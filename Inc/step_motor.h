/*
 * step_motor.h
 *
 *  Created on: Apr 15, 2021
 *      Author: xrj
 */

#ifndef INC_STEP_MOTOR_H_
#define INC_STEP_MOTOR_H_

#include "main.h"

typedef struct{
	GPIO_PinState b1:1;
	GPIO_PinState b2:1;
	GPIO_PinState b3:1;
	GPIO_PinState b4:1;
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
	TIM_HandleTypeDef *htimx;
}StepMotor_Connect;

typedef struct{
	StepMotor_Connect *conn;
	StepMotor_State *seq;
	int32_t seq_len;
	int32_t seq_i;        // 0 <= seq_i < seq_len
	int64_t total_rot;    // +=rot; each RunStep
	int32_t remain_step;  // --; each RunStep, -1 will always run;
	int32_t rot;          // 1:CCW, -1:CW, 0:stop
	void (*FinishCallback)(void*);  // include call `stop` by user
}StepMotor_Handle;


void StepMotor_LoadDefault(StepMotor_Handle *hsm);
void StepMotor_Init(StepMotor_Handle *hsm);
void StepMotor_setMode(StepMotor_Handle *hsm, StepMotor_Mode mode);
void StepMotor_Stop(StepMotor_Handle *hsm);
void StepMotor_set_us(StepMotor_Handle *hsm, int us, uint32_t steps);
void StepMotor_setSpeed(StepMotor_Handle *hsm, float deg_sec, float total_deg);
void StepMotor_RunStep(StepMotor_Handle *hsm);


__weak void DefaultFinishCallback(void *hsmx)
{
	UNUSED(hsmx);
}


#endif /* INC_STEP_MOTOR_H_ */
