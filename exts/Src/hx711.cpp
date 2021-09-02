/*
 * hx711.cpp
 *
 *  Created on: 2021年7月7日
 *      Author: xrj
 */

#include "delay.hpp"
#include "hx711.hpp"


#define HX711_US 5
HX711::HX711(C_Pin sck, C_Pin dout):sck(sck),dout(dout){};

void HX711::Init(HX711_Conv mode)
{
	this->mode = mode;
	sck.loadCfg(Pin_PP0);
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
	i32 ret=0;
	for(int i=0;i<24;i++){
		ret<<=1;
		sck.write_pin(Pin_Set);
		XDelayUs(HX711_US);
		ret|=dout.read_pin();
		sck.write_pin(Pin_Reset);
		XDelayUs(HX711_US);
	}
	for(int i=0;i<next;i++){
		sck.write_pin(Pin_Set);
		XDelayUs(HX711_US);
		sck.write_pin(Pin_Reset);
		XDelayUs(HX711_US);
	}
	if(ret&(1<<23)){
		ret|=0xff<<24;
	}
	return ret;
}

void HX711::wait()
{
	dout.wait_pin(Pin_Reset);
	XDelayUs(HX711_US);
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
