/*
 * dht11.cpp
 *
 *  Created on: 2021年6月7日
 *      Author: xrj
 */

#include "dht11.hpp"
#include "ops.hpp"
#include "delay.hpp"


DHT11::DHT11(C_Pin *pin):pin(*pin){};

DHT11_PackState DHT11::read_raw(DHT11_RAW *data)
{
	pin.loadCfg(Pin_PP0);
	XDelayMs(20);
	pin.loadCfg(Pin_InUp);
	XDelayUs(30);
	WAIT(Pin_Reset);
	WAIT(Pin_Set);
	u8* p = (u8*)data;
	for(int i=0;i<5;i++){
		i32 tmp=read_byte();
		if(tmp<0){ //error state
			return (DHT11_PackState)tmp;
		}else{
			*p++ = tmp;
		}
	}
    if(data->check != data->hum_H + data->hum_L + data->temp_H + data->temp_L){
        data->ps = DHT11_CheckSum_Err;
    }
    return data->ps;
}

i32 DHT11::read_byte()
{
	u8 ret=0;
	bool tmp;
	WAIT(Pin_Reset);  //wait to low state
	for(int i=7;i>=0;i--){
		WAIT(Pin_Set);  //wait to high state
#ifdef USE_VAR_SPLIT
		u32 t = pin.wait_count(Pin_Set, MIN_LOOPS, MAX_LOOPS);
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
		ret|=(tmp<<i);
	}
	return ret;
}

void DHT11::test(u32 *tH)
{
	pin.loadCfg(Pin_PP0);
	XDelayMs(20);
	pin.loadCfg(Pin_InUp);
	XDelayUs(30);
	WAIT(Pin_Reset);
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

float DHT11::hum(DHT11_RAW *data)
{
	float ret=data->hum_H + data->hum_L/10.0;
	if(data->hum_L & 0x80){
		ret*=-1;
	}
	return ret;
}

float DHT11::temp(DHT11_RAW *data)
{
	float ret=data->temp_H + data->temp_L/10.0;
	if(data->temp_L & 0x80){
		ret*=-1;
	}
	return ret;
}
