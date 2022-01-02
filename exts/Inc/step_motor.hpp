/*
 * step_motor.h
 *
 *  Created on: Apr 15, 2021
 *      Author: xrj
 */

#include "c_tim.hpp"
#if !defined(__STEP_MOTOR_HPP__) && \
	defined(__STM32_TIM_HPP__)
#define __STEP_MOTOR_HPP__

#include "mylibs_config.hpp"
#include "pins_manager.hpp"


/*
 * how to use the driver:
 * the driver need FreeRTOS binary semaphore.
 * the example use TIM2(32bit in STM32F4), you can change to other timer.
 *
 * 1. config by STM32Cube
 *     set GPIO Pins to Output mode and add User Label 'SMx'(x=1..4)
 *     set TIM2 clock source 'Internal Clock'
 *     enable TIM2 global interrupt.
 *
 * 2. create user_code.cpp/hpp files for run user codes, codes in the example are C++
 *
 * 3. copy below code to global:
 *     #include "main.h"
 *
 *     extern TIM_HandleTypeDef htim2;  //timer used for step motor
 *     PinCfg cfg = GPIO_GP_PP0 & OUT_2MHZ;
 *     ManagerPin sm_pins[4] = {        //GPIO Pins connect to motor driver ULN2003
 *     {C_Pin(SM1_GPIO_Port, SM1_Pin), true, InitCfg_Disable, NoLock, cfg, cfg},
 *     {C_Pin(SM2_GPIO_Port, SM2_Pin), true, InitCfg_Disable, NoLock, cfg, cfg},
 *     {C_Pin(SM3_GPIO_Port, SM3_Pin), true, InitCfg_Disable, NoLock, cfg, cfg},
 *     {C_Pin(SM4_GPIO_Port, SM4_Pin), true, InitCfg_Disable, NoLock, cfg, cfg}};
 *     GPIO_Conn sm_conn = GPIO_Conn(sm_pins, 4);
 *     StepMotor sm = StepMotor(sm_conn, (C_TIM*)&htim2);
 *
 * 4. call `sm.Init()` after `osKernelStart()`, init semaphore
 *
 * 5. call `sm.run_step()` in timer update event
 *     add to `void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)`
 *        \* USER CODE BEGIN Callback 1 *\
 *        if (htim->Instance == TIM2){
 *            StepMotor_TIMCallback();
 *        }
 *        \* USER CODE END Callback 1 *\
 *
 *    add to user_code.cpp, and extern "C" on .hpp file
 *        void StepMotor_TIMCallback()
 *        {
 *            sm.run_step();
 *        }
 *
 * 6. use the driver:
 *     sm.run_us(10000, 32, true);   // 10ms/step, 32steps, blocking mode
 *     sm.run_speed(10, 90, false);  // 10deg/sec, 90degs, no-blocking mode
 */

#define ROT_STATE_BKPREG_NUM 1

#pragma pack(1)
typedef struct{
	unsigned int b1:1;
	unsigned int b2:1;
	unsigned int b3:1;
	unsigned int b4:1;
}StepMotor_State;
#pragma pack()

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
	uint32_t* odr_bitband[4];
	C_TIM *ctim;
	const StepMotor_State *seq;
	int32_t seq_len;
	int32_t seq_i;        // 0 <= seq_i < seq_len
	int64_t total_rot;    // +=rot; each RunStep
	int32_t remain_step;  // --; each RunStep, -1 will always run;
	int32_t rot;          // 1:CCW, -1:CW, 0:stop
#ifdef USE_FREERTOS
	osSemaphoreId_t sem;
#endif
	uint32_t timeout;
	int16_t rot_state;   // save to RTC backup register, rot in 8step mode
	void setState(const StepMotor_State State);
public:
	void (*FinishCallback)(void*);  // callback of `stop`
	StepMotor(GPIO_Conn &conn, C_TIM *ctim);
	void Init();
	void setMode(StepMotor_Mode mode);
	void Stop();
	void wait();
	void run_us(uint32_t us, int32_t steps, bool blocking);
	void run_speed(float deg_sec, float total_deg, bool blocking);
	void run_step();
	int16_t get_rot_state();
};


__weak void DefaultFinishCallback(void *hsmx)
{
	UNUSED(hsmx);
}

#endif /* __STEP_MOTOR_HPP__ */
