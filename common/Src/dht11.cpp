/*
 * dht11.cpp
 *
 *  Created on: 2021年6月7日
 *      Author: xrj
 */

#include "dht11.hpp"
#include "delay.hpp"
#include "bit_band.h"
#include "ops.hpp"


DHT11::DHT11(Pin8b *pin):pin(*pin){};

DHT11_PackState DHT11::read_raw(DHT11_RAW *data)
{
	pin.loadCfg((PinCfg)(GPIO_GP_PP0|OUT_2MHZ));
	XDelayMs(20);
	pin.loadCfg(GPIO_In_Up);
	Delay_us(30);
	WAIT(false);
	WAIT(true);
	data->ps = read_bits(BIT_PTR(data, 0), 40);
	if(data->ps == DHT11_OK && data->check != data->hum_H + data->hum_L + data->temp_H + data->temp_L){
		data->ps == DHT11_CheckSum_Err;
	}
	return data->ps;
}

/*
 * MSB mode read bits
 * @param bb : bit-band address of last bit
 *             BIT_PTR(data+1, 0), `data` is pointer of DHT11_RAW
 * @param n  : number of bits, 40 for DHT11
 */
DHT11_PackState DHT11::read_bits(u32 *bb, u32 n)
{
	bb+=n;
	WAIT(false);  //wait to low state
	for(u32 i=0;i<n;i++){
		bb--;        //MSB first
		WAIT(true);  //wait to high state
#ifdef USE_VAR_SPLIT
		u32 t = pin.wait_count(true, MIN_LOOPS, MAX_LOOPS);
		if(t>=MAX_LOOPS){
			return DHT11_RxTimeout;
		}
		*bb = t > split;
#else
		Delay_us(SPILT01_US);
		*bb = pin.read_pin();
		if(*bb){//high level
			WAIT(false);
		}//else{already low level}
#endif
	}
	WAIT(true);
	return DHT11_OK;
}

u8 DHT11::read_byte()
{
	u8 ret=0;
	bool tmp;
	WAIT(false);  //wait to low state
	for(int i=7;i>=0;i--){
		WAIT(true);  //wait to high state
		Delay_us(SPILT01_US);
		tmp = pin.read_pin();
		if(tmp){//high level
			WAIT(false);
		}//else{already low level}
		ret|=(tmp<<i);
	}
	return ret;
}

void DHT11::test(u32 *tH)
{
	pin.loadCfg((PinCfg)(GPIO_GP_PP0|OUT_2MHZ));
	XDelayMs(20);
	pin.loadCfg(GPIO_In_Up);
	Delay_us(30);
	WAIT(false);
	WAIT(true);
	WAIT(false);
	for(int i=0;i<40;i++){
		WAIT(true);
		*tH = pin.wait_count(true, MIN_LOOPS, MAX_LOOPS);
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
