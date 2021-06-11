/*
 * timer.c
 *
 *  Created on: Apr 17, 2021
 *      Author: xrj
 */
#include "c_tim.hpp"

#include <stdlib.h>
#include "c_rcc.hpp"



TIM_TRGO TIM_CHx2TRGO(TIM_CHx ch)
{
	switch(ch){
	case TIM_Channel_1:
		return TIM_TRGO_oc1ref;
	case TIM_Channel_2:
		return TIM_TRGO_oc2ref;
	case TIM_Channel_3:
		return TIM_TRGO_oc3ref;
	case TIM_Channel_4:
		return TIM_TRGO_oc4ref;
	default:
		return TIM_TRGO_reset;
	}
}

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

void C_TIM::set_TGRO(TIM_TRGO trgo, bool ms_enable)
{
	TIM_MasterConfigTypeDef cfg;
	cfg.MasterOutputTrigger = trgo;
	cfg.MasterSlaveMode = ms_enable?TIM_MASTERSLAVEMODE_ENABLE:TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(this, &cfg);
}


void C_TIM::set_CountEnable(bool isEnable)
{
	if(isEnable){
		SET_BIT(this->Instance->CR1, TIM_CR1_CEN);
	}else{
		CLEAR_BIT(this->Instance->CR1, TIM_CR1_CEN);
	}
}

//max count of timer, TIM2,TIM5 are 32bit timer, will return 2^31-1, other return 2^16-1
u32 C_TIM::maxcount()
{
	return 0xffff;
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



//single channel class
TIM_CH::TIM_CH(C_TIM *htim, TIM_CHx Channel, bool allowCNT)
{
	this->htim = htim;
	this->Channel = Channel;
	this->allowCNT = allowCNT;
}

void TIM_CH::pluse_ns(u32 delay_ns, u32 pluse_ns, bool blocking)
{
	htim->pluse_ns(Channel, delay_ns, pluse_ns, blocking);
}

void TIM_CH::pluse_clk(u32 delay_clk, u32 pluse_clk, bool blocking)
{
	htim->pluse_clk(Channel, delay_clk, pluse_clk, blocking);
}

void TIM_CH::set_OCMode(TIM_OCMode mode)
{
	htim->set_OCMode(Channel, mode);
}

void TIM_CH::CCxChannelCmd(TIM_CCxE ChannelState)
{
	htim->CCxChannelCmd(Channel, ChannelState);
}


C_TIMEx::C_TIMEx(TIM_HandleTypeDef* htim)
{
	ctim = (C_TIM*)htim;
}

void C_TIMEx::set_callback(TIM_IT IT, void (*func)(void*), void* param)
{
	int i=__builtin_ctz(IT);
	if(i<0 or i>=N_IT){
		return;
	}
	callbacks[i]=func;
	params[i]=param;
}

void C_TIMEx::clear_callback(TIM_IT IT)
{
	set_callback(IT, nullptr, nullptr);
}

//ref to `C_TIMEx_ISR_func()`
void C_TIMEx::from_ISR()
{
	u32 sr=READ_REG(ctim->Instance->SR) & READ_REG(ctim->Instance->DIER) & 0xff;
	int i=-1;
	while(sr && i<N_IT){
		int di=__builtin_ctz(sr)+1;
		i+=di;
		sr>>=di;
		if(callbacks[i]){
			callbacks[i](params[i]);
		}
	}
}

/*
 * example:
 * in stm32f1xx_it.c add these code:
 *
 * typedef void C_TIMEx;
 * extern C_TIMEx etim2;  //"etim2" just example name
 * void C_TIMEx_ISR_func(C_TIMEx* etim);
 *
 * edit ISR func `void TIM2_IRQHandler(void)`:
 * call `C_TIMEx_ISR_func(&etim2);` before `HAL_TIM_IRQHandler(&htim2);`
 */
void C_TIMEx_ISR_func(C_TIMEx* etim)
{
	etim->from_ISR();
}
