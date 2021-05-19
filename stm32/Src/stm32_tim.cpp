/*
 * timer.c
 *
 *  Created on: Apr 17, 2021
 *      Author: xrj
 */
#include "c_tim.hpp"

#include <stdlib.h>
#include "c_rcc.h"


/*
 * @param trig:     TIM_TS_xxx  @ref: TIM_Trigger_Selection
 * @param polar:    TIM_ETRPOLARITY_(NON)INVERTED
 * @param prescale: TIM_ETRPRESCALER_DIVx (x=1,2,4,8)
 * @param filter:
 */
void C_TIM::set_Trig(u32 trig, u32 polar, u32 prescale, u32 filter)
{
	MODIFY_REG(this->Instance->SMCR, TIM_SMCR_ETP, polar);
	MODIFY_REG(this->Instance->SMCR, TIM_SMCR_ETPS, prescale);
	MODIFY_REG(this->Instance->SMCR, TIM_SMCR_ETF, filter);
	MODIFY_REG(this->Instance->SMCR, TIM_SMCR_TS, trig);
}

void C_TIM::set_ExtClk(bool isExtern)
{
	if(isExtern){
		SET_BIT(this->Instance->SMCR, TIM_SMCR_ECE);
	}else{
		CLEAR_BIT(this->Instance->SMCR, TIM_SMCR_ECE);
	}
}

//get bit masked clock div value
inline u32 C_TIM::get_clockdiv_sft()
{
	return __HAL_TIM_GET_CLOCKDIVISION(this);
}

//get 2^n clock div value
inline u32 C_TIM::get_clockdiv_2n()
{
	return __HAL_TIM_GET_CLOCKDIVISION(this) >> TIM_CR1_CKD_Pos;
}

inline u32 C_TIM::get_prescale()
{
	return READ_REG(this->Instance->PSC);
}

inline u32 C_TIM::get_autoload()
{
	return READ_REG(this->Instance->ARR);
}

inline void C_TIM::set_clockdiv_sft(u32 ckd)
{
	__HAL_TIM_SET_CLOCKDIVISION(this, ckd);
}


inline void C_TIM::set_clockdiv_2n(u32 ckd_2n)
{
	__HAL_TIM_SET_CLOCKDIVISION(this, ckd_2n << TIM_CR1_CKD_Pos);
}

inline void C_TIM::set_prescale(u32 psc)
{
	WRITE_REG(this->Instance->PSC, psc);
}

inline void C_TIM::set_autoload(u32 arr)
{
	WRITE_REG(this->Instance->ARR, arr);
}

uint64_t C_TIM::get_div(TIM_ClockLevel level)
{
	uint64_t div=1;
	//without break, exec code after case.
	switch(level){
	case TIM_ClkLv_AutoLoad:
		div *= (this->Instance->ARR + 1);
	case TIM_ClkLv_PSC16b:
		div *= (this->Instance->PSC + 1);
	case TIM_ClkLv_CKD:
		div <<= (__HAL_TIM_GET_CLOCKDIVISION(this) >> TIM_CR1_CKD_Pos);
	default:
		break;
	}
	return div;
}

Type_Hz C_TIM::get_Hz(TIM_ClockLevel level)
{
	float Hz = PeriphAddr2Freq(this->Instance);
	if(level >= TIM_ClkLv_Mult2){
		Hz *= 2.0;
	}
	return Hz/this->get_div(level);
}

Type_ns C_TIM::get_ns(TIM_ClockLevel level)
{
	uint64_t Hz = PeriphAddr2Freq(this->Instance);
	if(level >= TIM_ClkLv_Mult2){
		Hz *= 2;
	}
	return this->get_div(level)*1000000000UL/Hz;
}

/* set total div
 * @param div: fdiv of level `ClkLv_AutoLoad`
 * @retval: div setted, 0:div too high
 */
TypeDiv C_TIM::set_tdiv(TypeDiv div)
{
	TypeDiv mc = maxcount();
	uint32_t ckd = div/(mc+1)/0x10000 + 1;
	if(ckd > 4){
		return 0;  //div too high
	}
	ckd = ckd==3 ? 4 : ckd;
	div /= ckd;

	uint32_t psc = div/(mc+1) + 1;// psc as least value
	uint32_t psc2;
	uint32_t err2 = 0xffffffff;
	do{
		uint32_t al = (div+psc/2)/psc;
		uint32_t err = labs(al*psc - div);
        if(err < err2){
        	psc2 = psc;
        	err2 = err;
        	if(err == 0){
        		break;
        	}
        }
	}while(psc++ <= 0xffff);
	uint32_t al = (div+psc2/2)/psc2;
	TypeDiv div2 = (TypeDiv)ckd*psc2*al;
	ckd = __builtin_ctz(ckd) << TIM_CR1_CKD_Pos;
	__HAL_TIM_SET_CLOCKDIVISION(this, ckd);
	__HAL_TIM_SET_PRESCALER(this, psc2-1);
	__HAL_TIM_SET_AUTORELOAD(this, al-1);
	return div2;
}

//set Hz of clock `TIM_ClkLv_AutoLoad` to closest value
void C_TIM::set_Hz(Type_Hz Hz)
{
	uint64_t div = int(PeriphAddr2Freq(this->Instance)*2.0/Hz);
	this->set_tdiv(div);
}

//set ns of clock `TIM_ClkLv_AutoLoad` to closest value
void C_TIM::set_ns(Type_ns ns)
{
	uint64_t div = ns*PeriphAddr2Freq(this->Instance)*2/1000000000UL;
	this->set_tdiv(div);
}

void C_TIM::set_CountEnable(bool isEnable)
{
	if(isEnable){
		SET_BIT(this->Instance->CR1, TIM_CR1_CEN);
	}else{
		CLEAR_BIT(this->Instance->CR1, TIM_CR1_CEN);
	}
}

/*
 * set count mode.
 * @param mode: can be TIM_CountMode_(Up/Down/Cen1/Cen2/Cen3)
 */
inline void C_TIM::set_CountMode(TIM_CountMode mode)
{
	MODIFY_REG(this->Instance->CR1, TIM_CR1_DIR|TIM_CR1_CMS, mode);
}

//reset count value, upcounter set to 0, downcounter set to autoload value.
inline void C_TIM::reset_count()
{
	uint32_t cnt = __HAL_TIM_IS_TIM_COUNTING_DOWN(this)?__HAL_TIM_GET_AUTORELOAD(this):0;
	__HAL_TIM_SET_COUNTER(this, cnt);
}

//max count of timer, TIM2,TIM5 are 32bit timer, will return 2^31-1, other return 2^16-1
u32 C_TIM::maxcount()
{
	return 0xffff;
}


//operate single channels

//get single channel compare value
inline u32 C_TIM::get_comp(TIM_CHx Channel)
{
	return __HAL_TIM_GET_COMPARE(this, Channel);
}

//get single channel duty 0..1
inline float C_TIM::get_duty(TIM_CHx Channel)
{
	uint32_t al = __HAL_TIM_GET_AUTORELOAD(this);
	return (float)__HAL_TIM_GET_COMPARE(this, Channel)/al;
}

//set single channel compare value
inline void C_TIM::set_comp(TIM_CHx Channel, u32 comp)
{
	__HAL_TIM_SET_COMPARE(this, Channel, comp);
}

//set single channel compare value by duty 0..1
inline void C_TIM::set_duty(TIM_CHx Channel, float duty)
{
	uint32_t al = __HAL_TIM_GET_AUTORELOAD(this);
	__HAL_TIM_SET_COMPARE(this, Channel, duty*al);
}

//set 4 Channels compare value at a time, if compare == CompKeep, keep old compare value.
void C_TIM::set_comp4(u32 comp1, u32 comp2, u32 comp3, u32 comp4)
{
	if(comp1 != CompKeep){
		__HAL_TIM_SET_COMPARE(this, TIM_CHANNEL_1, comp1);
	}if(comp2 != CompKeep){
		__HAL_TIM_SET_COMPARE(this, TIM_CHANNEL_2, comp2);
	}if(comp3 != CompKeep){
		__HAL_TIM_SET_COMPARE(this, TIM_CHANNEL_3, comp3);
	}if(comp4 != CompKeep){
		__HAL_TIM_SET_COMPARE(this, TIM_CHANNEL_4, comp3);
	}
}

//set 4 Channels duty at a time, if duty not in 0<=x<=1, keep old compare value.
void C_TIM::set_duty4(float duty1, float duty2, float duty3, float duty4)
{
	uint32_t al = __HAL_TIM_GET_AUTORELOAD(this);
	if(0<=duty1 && duty1<=1){
		__HAL_TIM_SET_COMPARE(this, TIM_CHANNEL_1, duty1*al);
	}if(0<=duty2 && duty2<=1){
		__HAL_TIM_SET_COMPARE(this, TIM_CHANNEL_2, duty2*al);
	}if(0<=duty3 && duty3<=1){
		__HAL_TIM_SET_COMPARE(this, TIM_CHANNEL_3, duty3*al);
	}if(0<=duty4 && duty4<=1){
		__HAL_TIM_SET_COMPARE(this, TIM_CHANNEL_4, duty4*al);
	}
}

//OnePluse Mode clocks after 16bit prescale
void C_TIM::pluse_clk(TIM_CHx Channel, u32 delay_clk, u32 pluse_clk, bool blocking)
{
	CLEAR_BIT(this->Instance->CR1, TIM_CR1_CEN);
	this->set_CountMode(TIM_CountMode_Up);
	__HAL_TIM_SET_COUNTER(this, 0);
	SET_BIT(this->Instance->CR1, TIM_CR1_OPM);
	this->set_OCMode(Channel, TIM_OCMode_PWM2);
	this->set_autoload(delay_clk + pluse_clk);
	this->set_comp(Channel, delay_clk);
	SET_BIT(this->Instance->CR1, TIM_CR1_CEN);
	if(blocking){
		while(READ_BIT(this->Instance->CR1, TIM_CR1_CEN));
	}
}

//OnePluse Mode use nanoseconds unit
void C_TIM::pluse_ns(TIM_CHx Channel, u32 delay_ns, u32 pluse_ns, bool blocking)
{
	uint32_t psc_ns = get_ns(TIM_ClkLv_PSC16b);
	this->pluse_clk(Channel, delay_ns/psc_ns, pluse_ns/psc_ns, blocking);
}

//force output active state.
void C_TIM::set_OCMode(TIM_CHx Channel, TIM_OCMode mode)
{
	switch(Channel){
	case TIM_CHANNEL_1:
		MODIFY_REG(this->Instance->CCMR1, TIM_CCMR1_OC1M, mode);
		break;
	case TIM_CHANNEL_2:
		MODIFY_REG(this->Instance->CCMR1, TIM_CCMR1_OC2M, mode<<8);
		break;
	case TIM_CHANNEL_3:
		MODIFY_REG(this->Instance->CCMR2, TIM_CCMR2_OC3M, mode);
		break;
	case TIM_CHANNEL_4:
		MODIFY_REG(this->Instance->CCMR2, TIM_CCMR2_OC4M, mode<<8);
		break;
	default:
		break;
	}
}

void C_TIM::CCxChannelCmd(TIM_CHx Channel, TIM_CCxE ChannelState)
{
	TIM_CCxChannelCmd(this->Instance, Channel, ChannelState);
}
