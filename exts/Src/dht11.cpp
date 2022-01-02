/*
 * dht11.cpp
 *
 *  Created on: 2021年6月7日
 *      Author: xrj
 */

#include "dht11.hpp"
#ifdef __DHT11_HPP__

#include "ops.hpp"
#include "delay.hpp"


DHT11::DHT11(C_Pin pin):pin(pin)
{
    //pin.loadXCfg(GPIO_GP_OD1);
}

DHT11_PackState DHT11::read_raw()
{
	pin.write_pin(Pin_Reset);
	XDelayMs(20);  //拉低>18ms
	pin.write_pin(Pin_Set);
	
	//响应信号
	WAIT(Pin_Set);
	WAIT(Pin_Reset);
	WAIT(Pin_Set);
	
	raw.hum_H = read_byte();
	raw.hum_L = read_byte();
	raw.temp_H = read_byte();
	raw.temp_L = read_byte();
	raw.check = read_byte();
    if(raw.check != raw.hum_H + raw.hum_L + raw.temp_H + raw.temp_L){
        raw.ps = DHT11_CheckSum_Err;
    }else{
        raw.ps = DHT11_OK;
    }
    return raw.ps;
}

i32 DHT11::read_byte()
{
	u8 ret=0;
	bool tmp;
	WAIT(Pin_Reset);  //wait to low state
	for(int i=0;i<8;i++){
		WAIT(Pin_Set);  //wait to high state
#ifdef USE_VAR_SPLIT
		u32 t = pin.wait_count(Pin_Reset, MIN_LOOPS, MAX_LOOPS);
		if(t>=MAX_LOOPS){
			return DHT11_RxTimeout;
		}
		tmp = (t>split);
#else
		XDelayUs(SPILT01_US);
		tmp = pin.read_pin();
		if(tmp){//high level
			WAIT(Pin_Reset);
		}//else{already low level}
#endif
		ret<<=1;
		ret|=tmp;
	}
	return ret;
}

#ifdef USE_VAR_SPLIT
void DHT11::test(u32 *tH)
{
	pin.write_pin(Pin_Reset);
	XDelayMs(20);  //拉低>18ms
	pin.write_pin(Pin_Set);
	
	//响应信号
	WAIT(Pin_Set);
	WAIT(Pin_Reset);
	for(int i=0;i<40;i++){
		WAIT(Pin_Set);
		*tH = pin.wait_count(Pin_Set, MIN_LOOPS, MAX_LOOPS);
		if(*tH >= MAX_LOOPS){
			*tH = 0;
		}
		tH++;
	}
}

u32 DHT11::carlib_split()
{
	u32 tH[40];
	u32 *p;
	test(tH);
	u32 spt=sum(tH, 40);

	//statistics not zero values
	u32 nn0 = 0;
	p = tH;
	for(int i=0;i<40;i++){
		if(*p++ != 0){
			nn0++;
		}
	}

	//fill mean value to zero values
	spt/=nn0;
	p = tH;
	for(int i=0;i<40;i++){
		if(*p == 0){
			*p = spt;
		}
		p++;
	}

	//iteration get split value
	for(int i=0;i<3;i++){
		p = tH;
		u32 s0=0, s1=0;
		u32 n0=0, n1=0;
		for(int j=0;j<40;j++){
			if(*p<spt){
				s0 += *p;
				n0++;
			}else{
				s1 += *p;
				n1++;
			}
			p++;
		}
		spt = (s0/n0+s1/n1)/2;
	}
	split = spt-2;
	return split;
}
#endif

#ifdef DHT_INTEGER
u16 DHT11::hum_0p1()
{
	return raw.hum_H*10 + raw.hum_L;
}

i16 DHT11::temp_0p1()
{
	i16 tmp = ((i16)raw.temp_H)*10 + (raw.temp_L&0x7f);
	if(raw.temp_L&0x80){
		tmp *= -1;
	}
	return tmp;
}
#endif

#ifdef DHT_FLOAT
float DHT11::hum()
{
	float ret=raw.hum_H + raw.hum_L/10.0f;
	return ret;
}

float DHT11::temp()
{
	float ret=raw.temp_H + raw.temp_L/10.0f;
	if(raw.temp_L & 0x80){
		ret*=-1;
	}
	return ret;
}
#endif

#endif
