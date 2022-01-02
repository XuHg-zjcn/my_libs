/*
 * aht2x.cpp
 *
 *  Created on: 2021年11月11日
 *      Author: xrj
 */

#include "aht2x.hpp"
#ifdef __AHT2X_HPP__

#include "delay.hpp"

AHT2x::AHT2x(AHT_I2C_Dev *i2c):i2c(i2c){};

AHT_Stat AHT2x::Init()
{
	i2c->Mem_read(0x71, data, 1);
	if(data[0]&0x80){
		return AHT_Busy;
	}else{
		return AHT_OK;
	}
}

#define POLY 0x31
u8 AHT2x::CRC8()
{
	u8 crc = 0xff;
	for(u8 i=0;i<6;i++){
		crc ^= data[i];
		for(u8 j=0;j<8;j++){
			if(crc&0x80){
				crc=(crc<<1)^POLY;
			}else{
				crc<<=1;
			}
		}
	}
	return crc;
}

void AHT2x::trig()
{
	u8 commd[3]={0xAC, 0x33, 0x00};
	i2c->send(commd, 3);
}

AHT_Stat AHT2x::read()
{
	//TODO: 不读取全部，通过第一个字节判断是否忙，立即停止
	i2c->recv(data, 7);
	if(data[0]&0x80){
		return AHT_Busy;
	}if(CRC8()!=data[6]){
		return AHT_CRCErr;
	}
	return AHT_OK;
}

AHT_Stat AHT2x::trig_read()
{
	trig();
	XDelayMs(80);
	while(true){
		if(read()==AHT_OK){
			return AHT_OK;
		}
		XDelayMs(10);
	}
}

i32 AHT2x::mCTemp()
{
	u32 tmp = (data[3]&0x0f)<<16 | data[4]<<8 | data[5];
	tmp *= 3125;
	tmp >>= 14;
	return ((i32)tmp)-50000;
}

u32 AHT2x::mHum()
{
	u32 hum = data[1]<<12 | data[2]<<4 | data[3]>>4;
	hum *= 3125;
	hum >>= 15;
	return hum;
}

#endif
