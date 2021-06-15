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


DC_Motor::DC_Motor(TIM_CH &tim_pwm, C_ADC *cadc, ADC_CHx CH_Current):
	tim_pwm(tim_pwm), tim_spd(tim_pwm)
{
	this->count = 0;
	this->cadc = cadc;
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

void DC_Motor::stop()
{
	tim_pwm.set_OCMode(TIM_OCMode_Forced_InActive);
}

void DC_Motor::run_pwm()
{
	tim_pwm.set_OCMode(TIM_OCMode_PWM1);
	tim_pwm.CCxChannelCmd(TIM_CCx_Enable);
}

u32 DC_Motor::t_value(BuffHeadRead head)
{
	u32 Sxx = ((NZIP*NZIP-1)/12)*NZIP;
	cadc->DMA_once(NSAMP, true);
	u16 *p = (u16*)head.get_frames(NSAMP);
	filter(p, NSAMP, 300);
	mean_zip(p, NSAMP/NZIP, NZIP, 5);
	u32 Eiy = isum(p, NZIP);
	u32 Ey = sum(p, NZIP);
	u32 Ey2 = sum2<u16,u32>(p, NZIP);
	i32 Sxy = Eiy - Ey/2*(NZIP-1);
	u64 Sxy2 = (i64)Sxy*(i64)Sxy;
	u32 Syy = Ey2 - Ey/NZIP*Ey;
	i32 b_ = Sxy/(i32)(Sxx/1000);
	i64 t = Sxy2*(NZIP-2)*10000LL/((i64)Syy*(i64)Sxx-Sxy2);
	return uint_sqrt(t);
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
	cadc->load_regular_seq(&sseq);
	cadc->conn_buff(&buff.w_head);
	cadc->set_SR_sps(128000);
	cadc->DMA_once(NSAMP, true);
	p = (u16*)head.get_frames(NSAMP);
	u32 s0 = sum(p, NSAMP);
	tim_pwm.CCxChannelCmd(TIM_CCx_Enable);
	int i=0;
	while(i<9){
		while(t_value(head)>50);
		for(i=0;i<10 && t_value(head)<200;i++);
	}
	XDelayMs(500);
	*led = 0;
	cadc->DMA_once(NSAMP, true);
	p = (u16*)head.get_frames(NSAMP);
	u32 s1 = sum(p, NSAMP);
	u32 max = s0 + xfact((s1-s0), 102, 100); //使用积分检测
	char str[20];
	oled.setVHAddr(Horz_Mode, 0, 127, 0, 1);
	snprintf(str, 20, " s0:%5d", s0/409);
	oled.text_5x7(str);
	oled.setVHAddr(Horz_Mode, 0, 127, 1, 2);
	snprintf(str, 20, " s1:%5d", s1/409);
	oled.text_5x7(str);
	oled.setVHAddr(Horz_Mode, 0, 127, 2, 3);
	snprintf(str, 20, "max:%5d", max/409);
	oled.text_5x7(str);
	while(1){
		cadc->DMA_once(NSAMP, true);
		p = (u16*)head.get_frames(NSAMP);
		u32 s2 = sum(p, NSAMP);
		oled.setVHAddr(Horz_Mode, 0, 127, 4, 5);
		snprintf(str, 20, " s2:%5d", s2/409);
		oled.text_5x7(str);
		if(s2 > max){
			tim_pwm.CCxChannelCmd(TIM_CCx_Disable);
			return;
		}
	}
}



//@param addr: page addr of Flash, 0x080***00
TestSave::TestSave(DC_Motor *motor, TestRes *addr)
{
	this->motor = motor;
	this->addr = addr;
}

TestSave::TestSave(DC_Motor *motor, TestRes *addr, u32 n, u32 ms, float maxduty)
{
	this->motor = motor;
	this->addr = addr;
	this->n = n;
	this->ms = ms;
	this->maxduty = maxduty;
}

void TestSave::Eraser()
{
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef pEraseInit;
	pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	pEraseInit.Banks = FLASH_BANK_1;
	pEraseInit.PageAddress = (u32)this->addr;
	pEraseInit.NbPages = 1;
	u32 err;
	HAL_FLASHEx_Erase(&pEraseInit, &err);
	HAL_FLASH_Lock();
	if(err != 0xFFFFFFFF){
		X_ErrorLog(__FILE__, __LINE__);
	}
}

void TestSave::TestStart()
{
	Eraser();
	u16 *p;
	TestRes *addr2 = addr;
	motor->tim_pwm.CCxChannelCmd(TIM_CCx_Disable);
	Buffer buff=Buffer(2);
	buff.Init();
	buff.remalloc(NSAMP);
	BuffHeadRead head = BuffHeadRead(buff.r_heads, buff.r_heads.new_head());
	motor->cadc->conn_buff(&buff.w_head);
	XDelayMs(100);
	motor->cadc->DMA_once(NSAMP, true);
	p = (u16*)head.get_frames(NSAMP);
	ADCv_I0 = (float)sum(p, NSAMP)/NSAMP;
	for(u32 i=0;i<n;i++){
		float duty = (maxduty/n)*i;
		motor->setDuty(duty);
		XDelayMs(ms);
		motor->cadc->DMA_once(NSAMP, true);
		p = (u16*)head.get_frames(NSAMP);
		TestRes tr;
		tr.curr = value_clip(sum(p, NSAMP)>>12, 0, MEAN_Msk);
		tr.curr |= value_clip(istd(p, NSAMP), 0, STD_Msk)<<STD_Sft;
		tr.spd = 0;
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (u32)addr2++, *(u32*)(&tr));
	}
}
