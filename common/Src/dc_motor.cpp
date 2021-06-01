/*
 * dc_motor.cpp
 *
 *  Created on: 2021年5月28日
 *      Author: xrj
 */

#include <ops.hpp>
#include "dc_motor.hpp"
#include "x_logs.hpp"

extern u32* led;

DC_Motor::DC_Motor(TIM_CH &tim_pwm, C_ADCEx *hadc, ADC_CHx CH_Current):
	tim_pwm(tim_pwm), tim_spd(tim_pwm)
{
	this->count = 0;
	this->hadc = hadc;
	this->CH_Current = CH_Current;
	this->CH_Voltage = CH_Current;
}

/*
 * tL, tH <= MIN_NS
 *
 * tH/(tL+tH) == duty  always OK
 *
 */
void DC_Motor::setDuty(float duty)
{
	if(duty<DUTY_MIN){
		tim_pwm.set_OCMode(TIM_OCMode_Forced_InActive);
		return;
	}if(duty>DUTY_MAX){
		duty = DUTY_MAX;
	}
	uint32_t Hz = PWM_HZ;
	uint32_t t = 1000000000/Hz; //单位ns
	uint32_t tL = t*(1.0-duty);
	uint32_t tH = t*duty;
	if(tL < MIN_NS && tH < MIN_NS){      //设置错误
		X_ErrorLog(__FILE__, __LINE__);  //t_MIN*f_PWM >= 0.5
	}else if(tH < MIN_NS){
		tH = MIN_NS;
		t = tH/duty;
	}else if(tL < MIN_NS){
		tL = MIN_NS;
		t = tL/(1.0-duty);
	}
	Hz = 1000000000/t;
	tim_pwm.set_Hz(Hz);
	tim_pwm.set_duty(duty);
}

void DC_Motor::wait_OK()
{

}

void DC_Motor::run_monitor(ControlConfig &cfg, SSD1306 &oled)
{
	u16 *p;
	tim_pwm.CCxChannelCmd(TIM_CCx_Disable);
	*led = 1;
	HAL_Delay(100);
	ADC_aSamp samp[2];
	ADC_SampSeq sseq={0, samp};
	if(CH_Current){
		samp[sseq.len++].CHx = CH_Current;
	}/*if(CH_Voltage){
		samp[sseq.len++].CHx = CH_Voltage;
	}*/
	Buffer buff=Buffer(sseq.len*2);
	buff.Init();
	buff.remalloc(NSAMP);
	BuffHeadRead head = BuffHeadRead(buff.r_heads, buff.r_heads.new_head());
	hadc->load_regular_seq(&sseq);
	hadc->conn_buff(&buff.w_head);
	hadc->set_SR_sps(128000);
	hadc->DMA_once(NSAMP, true);
	p = (u16*)head.get_frames(NSAMP);
	u32 s0 = sum(p, NSAMP);
	tim_pwm.CCxChannelCmd(TIM_CCx_Enable);
	u32 Sxx = ((NZIP*NZIP-1)/12)*NZIP;
	i64 t = 10000;
	while(t>50){
		hadc->DMA_once(NSAMP, true);
		p = (u16*)head.get_frames(NSAMP);
		filter(p, NSAMP, 300);
		mean_zip(p, NSAMP/NZIP, NZIP, 5);
		u32 Eiy = isum(p, NZIP);
		u32 Ey = sum(p, NZIP);
		u32 Ey2 = sum2<u16,u32>(p, NZIP);
		i32 Sxy = Eiy - Ey/2*(NZIP-1);
		u64 Sxy2 = (i64)Sxy*(i64)Sxy;
		u32 Syy = Ey2 - Ey/NZIP*Ey;
		i32 b_ = Sxy/(i32)(Sxx/1000);
		t = Sxy2*(NZIP-2)*10000LL/((i64)Syy*(i64)Sxx-Sxy2);
		t = u64_sqrt(t);
		/*if(Sxy < 0){
			t *= -1;
		}*/
	}
	HAL_Delay(500);
	*led = 0;
	hadc->DMA_once(NSAMP, true);
	p = (u16*)head.get_frames(NSAMP);
	u32 s1 = sum(p, NSAMP);
	u32 max = s0 + xfact((s1-s0), 1001, 1000); //使用积分检测
	while(1){
		hadc->DMA_once(NSAMP, true);
		p = (u16*)head.get_frames(NSAMP);
		u32 s2 = sum(p, NSAMP);
		if(s2 > max){
			tim_pwm.CCxChannelCmd(TIM_CCx_Disable);
			return;
		}
	}
}
