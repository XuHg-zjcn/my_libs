/*
 * smg.cpp
 *
 *  Created on: 2021年6月2日
 *      Author: xrj
 */

#include "smg8.hpp"
#include "font_smg8.h"
#include "myints.h"

SMG8::SMG8():buf({0,0,0,0}),index(0)
{
	C_Pin pseg[8] = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}};
	seg = new GPIO_Conn(pseg, 8, true, InitCfg_Disable, NoLock, GPIO_GP_OD1, GPIO_In_Up);
	C_Pin pxb[4] = {{1, 3}, {1, 4}, {1, 5}, {1, 8}};
	xb = new GPIO_Conn(pxb, 4, true, InitCfg_Disable, NoLock, GPIO_GP_PP0, GPIO_GP_PP0);
	colon = new C_Pin(1, 9);
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
	colon->write_pin(Pin_Reset);
}

void SMG8::connRTC(C_RTC* rtc)
{
	this->rtc = rtc;
}

void SMG8::connTIM(C_TIMEx* etim)
{
	this->etim = etim;
}

/*
 * @param i0: index of start
 * @param i1: index of end+1
 * @param num: number to show
 * @param point: 0:don't show, otherwise:point at n th number
 * @param fill0: is fill 0 at head?
 */
void SMG8::showNum(int i0, int i1, int num, int point, bool fill0)
{
	int i=i1-i0;
	if(num<0){
		i-=1;
	}
	u32 m=1;
	for(;i>0;i--){
		m*=10;
	}
	if(abs(num)>=m){
		return;
	}
	uint8_t *p = &buf[i1-1];
	do{
		*p-- = fontsmg8[num%10];
		num/=10;
	}while(num!=0);
	u8 fill = fill0 ? fontsmg8[0] : 0;
	while(p >= buf+i0){
		*p-- = fill;
	}
	if(point){
		buf[i0+point-1] |= 0x80;
	}
}

void SMG8::showNum(int num1, int point, int num2)
{
	showNum(0, point, num1, point, false);
	showNum(point, 4, num2, 0, true);
}

void SMG8::showNum(float num)
{
	int32_t tmp=flog10(num)+1;
	if(tmp<0){
		tmp=1;
	}else if(tmp>(num>0?4:3)){
		return; //太大
	}
	showNum(num, tmp);
}

void SMG8::showNum(float num, int point)
{
	u32 mul = 1;
	for(int i=point;i<4;i++){
		mul*=10;
	}
	int n2=(int)num;
	showNum(n2, point, (int)((num-n2)*mul));
}

void SMG8::showTime(int n, PinState c, int m)
{
	showNum(0, 2, n, 0, false);
	showNum(2, 4, m, 0, true);
	colon_state = c;
}

void SMG8::showCurrYear()
{
	showNum(rtc->get_tm()->tm_year+1900);
}

void SMG8::showCurrDate()
{
	struct tm *tm = rtc->get_tm();
	showNum(0, 2, tm->tm_mon+1, 2, false);
	showNum(2, 4, tm->tm_mday, 0, false);
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
	etim->ctim->set_ns((us_on+us_off)*1000);
	etim->ctim->Base_Start_IT();
	etim->clear_callback(TIM_IT_all);
	etim->set_callback(TIM_IT_update, &CTimerFunc, this);
	if(us_off != 0){
		etim->ctim->set_duty(TIM_Channel_1, (float)us_on/(us_on+us_off));
		etim->ctim->PWM_Start_IT(TIM_Channel_1);
		etim->set_callback(TIM_IT_cc1, &Coff, this);
	}
}

/*
 * callback of RTC second interrupt.
 * note: NVIC RTC priority should lower than TIM.
 */
void SMG8::RTCSecondCallback()
{
	struct tm *tm2 = rtc->get_tm();
	this->showTime(tm2->tm_hour, tm2->tm_sec%2==0?Pin_Set:Pin_Reset, tm2->tm_min);
}
