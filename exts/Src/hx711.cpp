/*
 * hx711.cpp
 *
 *  Created on: 2021年7月7日
 *      Author: xrj
 */

#include "delay.hpp"
#include "hx711.hpp"

HX711::HX711(C_Pin sck, C_Pin dout):sck(sck),dout(dout){};

void HX711::Init(HX711_Conv mode)
{
	this->mode = mode;
	this->loops = HAL_RCC_GetSysClockFreq()*2/(1000000*clk_loop);
	sck.loadCfg(Pin_PP0, 2);
	dout.loadCfg(Pin_InFlt);
	i32 sum=0;
	for(int i=0;i<8;i++){
		wait();
		sum += read_raw(HX711_CHA_128);
		XDelayMs(100);
	}
	val_at0 = sum/8;
	kg_LSB = -7.68e-5f;
}

i32 HX711::read_raw(HX711_Conv next)
{
	i32 ret;
	for(int i=0;i<24;i++){
		ret<<=1;
		sck.write_pin(Pin_Set);
		Delay_loopN(loops);
		ret|=dout.read_pin();
		sck.write_pin(Pin_Reset);
		Delay_loopN(loops);
	}
	for(int i=0;i<next;i++){
		sck.write_pin(Pin_Set);
		Delay_loopN(loops);
		sck.write_pin(Pin_Reset);
		Delay_loopN(loops);
	}
	if(ret&(1<<23)){
		ret|=0xff<<24;
	}
	return ret;
}

void HX711::wait()
{
	dout.wait_pin(Pin_Reset);
	Delay_loopN(loops);
}

i32 HX711::block_raw()
{
	wait();
	return read_raw(mode);
}

float HX711::read_kg()
{
	return (block_raw()-val_at0)*kg_LSB;
}
