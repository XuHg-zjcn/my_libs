/*
 * smg.cpp
 *
 *  Created on: 2021年6月2日
 *      Author: xrj
 */

#include "smg8.hpp"
#include "font_smg8.h"

SMG8::SMG8():buf({0,0,0,0}),index(0)
{
	Pin8b pseg[8] = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}};
	seg = new GPIO_Conn(pseg, 8, true, InitCfg_Disable, NoLock, GPIO_GP_OD1, GPIO_In_Up);
	Pin8b pxb[4] = {{1, 3}, {1, 4}, {1, 5}, {1, 8}};
	xb = new GPIO_Conn(pxb, 4, true, InitCfg_Disable, NoLock, GPIO_GP_PP0, GPIO_GP_PP0);
	colon = new Pin8b(1, 9);
}

void SMG8::LightMode()
{
	seg->Enable();
	xb->Enable();
	colon->loadCfg(GPIO_GP_PP0);
}

void SMG8::DarkMode()
{
	seg->Disable();
	xb->Disable();
	colon->loadCfg(GPIO_In_Down);
}

void SMG8::off()
{
	xb->WritePins(0);
	colon->write_pin(false);
}

void SMG8::connRTC(C_RTC* rtc)
{
	this->rtc = rtc;
}

void SMG8::connTIM(C_TIM* tim)
{
	this->tim = tim;
}

void SMG8::showNum(int num, int point)
{
	if(num>=10000 || num <=-1000){
		return;
	}
	uint8_t *p = &buf[3];
	do{
		*p-- = fontsmg8[num%10];
		num/=10;
	}while(num!=0);
	while(p >= buf){
		*p-- = 0;
	}
}

void SMG8::showTime(int n, bool c, int m)
{
	buf[0] = (n>9)?fontsmg8[(n/10)]:0;
	buf[1] = fontsmg8[n%10];
	buf[2] = fontsmg8[m/10];
	buf[3] = fontsmg8[m%10];
	colon_state = c;
}

/*
 * need call `RTCSecondCallback` in `HAL_RTCEx_RTCEventCallback`
 */
void SMG8::startTimeDymtic()
{
	rtc->EnableIT(RTC_IT_SEC);
}

void SMG8::stopTimeDymitc()
{
	rtc->DisableIT(RTC_IT_SEC);
}

void SMG8::TimerFunc()
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
	((SMG8*)obj)->TimerFunc();
}

void Coff(void* obj)
{
	((SMG8*)obj)->off();
}

/*
    stop run if FreeRTOS is busy, can change priority
    not advice use, is too frequently.
*/
#ifdef USE_FREERTOS
void SMG8::FreeRTOSTimer(uint32_t ms_on, uint32_t ms_off)
{
	osTimerAttr_t attr = { .name = "smg8_tim1" };
	osTimerId_t tid1 = osTimerNew(&CTimerFunc, osTimerPeriodic, this, &attr);
	osTimerStart(tid1, ms_on+ms_off);
	if(ms_off != 0){
		osTimerAttr_t attr = { .name = "smg8_tim2" };
		osTimerId_t tid2 = osTimerNew(&Coff, osTimerPeriodic, this, &attr);
		osDelay(ms_on);
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
void SMG8::HardWareTimer(uint32_t us_on, uint32_t us_off)
{
	tim->set_ns((us_on+us_off)*1000);
	tim->Base_Start_IT();
	if(us_off != 0){
		tim->set_duty(TIM_Channel_1, (float)us_on/(us_on+us_off));
		tim->PWM_Start_IT(TIM_Channel_1);
	}
}

/*
 * callback of RTC second interrupt.
 * note: NVIC RTC priority should lower than TIM.
 */
void SMG8::RTCSecondCallback()
{
	RTC_TimeTypeDef sTime;
	rtc->GetTime(&sTime, RTC_FORMAT_BIN);
	this->showTime(sTime.Hours, sTime.Seconds%2==0, sTime.Minutes);
}
