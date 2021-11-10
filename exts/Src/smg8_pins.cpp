/*
 * smg_pins.cpp
 *
 *  Created on: Jul 1, 2021
 *      Author: xrj
 */

#include "smg8_pins.hpp"

SMG8_Pins::SMG8_Pins()
{
	C_Pin pseg[8] = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}};
	seg = new GPIO_Conn(pseg, 8, true, InitCfg_Disable, NoLock, Pin_OD0, Pin_InUp);
	C_Pin pxb[4] = {{1, 3}, {1, 4}, {1, 5}, {1, 8}};
	xb = new GPIO_Conn(pxb, 4, true, InitCfg_Disable, NoLock, Pin_PP0, Pin_PP0);
	colon = new C_Pin(1, 9);
	index = 0;
	buf = (u8*)XMalloc(4);
}

void SMG8_Pins::LightMode()
{
	seg->Enable();
	xb->Enable();
	colon->loadCfg(Pin_PP0);
}

void SMG8_Pins::DarkMode()
{
	seg->Disable();
	xb->Disable();
	colon->loadCfg(Pin_PP0);
}

void SMG8_Pins::off()
{
	xb->WritePins(0);
	colon->write_pin(Pin_Reset);
}

#ifdef INC_STM32_TIM_HPP_
void SMG8_Pins::connTIM(C_TIM* ctim)
{
	this->ctim = ctim;
}
#endif

void SMG8_Pins::TimerFunc()
{
	seg->WritePins(~0);
	xb->WritePins(1<<index);
	seg->WritePins(~buf[index]);
	index++;
	if(index>=4){
		index=0;
	}
	colon->write_pin(colon_state);
}

void CTimerFunc(void* obj)
{
	((SMG8_Pins*)obj)->TimerFunc();
}

void Coff(void* obj)
{
	((SMG8_Pins*)obj)->off();
}

/*
    stop run if FreeRTOS is busy, can change priority
    not advice use, is too frequently.
*/
#ifdef USE_FREERTOS
void SMG8_Pins::FreeRTOSTimer(uint32_t ms_on, uint32_t ms_off)
{
	osTimerAttr_t attr = { .name = "smg8_tim1" };
	osTimerId_t tid1 = osTimerNew(&CTimerFunc, osTimerPeriodic, this, &attr);
	osTimerStart(tid1, ms_on+ms_off);
	if(ms_off != 0){
		osTimerAttr_t attr = { .name = "smg8_tim2" };
		osTimerId_t tid2 = osTimerNew(&Coff, osTimerPeriodic, this, &attr);
		XDelayMs(ms_on);
		osTimerStart(tid2, ms_on+ms_off);
	}
}
#endif

/*
 * @param us_on: microsecond of on
 * @param us_off: microsecond of off
 *
 * call TimerFunc() in HAL_TIM_PWM_PulseFinishedCallback
 * call off() HAL_TIM_PWM_PulseFinishedCallback
 */
#ifdef INC_STM32_TIM_HPP_
void SMG8_Pins::HardWareTimer(uint32_t us_on, uint32_t us_off)
{
	ctim->set_ns((us_on+us_off)*1000);
	ctim->Base_Start_IT();
	ctim->clear_callback(TIM_IT_all);
	ctim->set_callback(TIM_IT_update, &CTimerFunc, this);
	if(us_off != 0){
		ctim->set_duty(TIM_Channel_1, (float)us_on/(us_on+us_off));
		ctim->PWM_Start_IT(TIM_Channel_1);
		ctim->set_callback(TIM_IT_cc1, &Coff, this);
	}
}
#endif
