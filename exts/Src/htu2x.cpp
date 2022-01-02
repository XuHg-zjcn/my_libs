/*
 * htu2x.cpp
 *
 *  Created on: 2021年11月12日
 *      Author: xrj
 */

#include "htu2x.hpp"
#ifdef __HTU2X_HPP__

#include "delay.hpp"

HTU2x::HTU2x(HTU_I2C_Dev *i2c):i2c(i2c){};

#ifdef HOLD_READ
//TODO: 修复HOLD模式
u16 HTU2x::Hold_Read(HTU2x_Commd commd)
{
	u8 data[3];
	i2c->Mem_read((u8)commd, data, 3);
	return (data[0]<<8) | (data[1]&0xfc);
}
#endif

u16 HTU2x::NoHold_Read(HTU2x_Commd commd, uint32_t wait)
{
	u8 data[3];
	i2c->send((u8*)&commd, 1);
	XDelayMs(wait);
	i2c->recv(data, 3);
	return (data[0]<<8) | (data[1]&0xfc);
}

i16 HTU2x::Read_Temp0p01C()
{
#ifdef HOLD_READ
	u32 tmp = Hold_Read(HTU_Temp_NoHold);
#else
	u32 tmp = NoHold_Read(HTU_Temp_NoHold, 50);
#endif
	tmp *= 17572;
	tmp >>= 16;
	return (i16)tmp - 4685;
}

i16 HTU2x::Read_Hum0p01()
{
#ifdef HOLD_READ
	u32 tmp = Hold_Read(HTU_Temp_NoHold);
#else
	u32 tmp = NoHold_Read(HTU_Hum_NoHold, 16);
#endif
	tmp *= 12500;
	tmp >>= 16;
	return (i16)tmp - 500;
}

void HTU2x::Write_UserReg(HTU2x_UserReg reg)
{
	i2c->Mem_read(HTU_Write_UserReg, (u8*)&reg, 1);
}

HTU2x_UserReg HTU2x::Read_UserReg()
{
	u8 data;
	i2c->Mem_read(HTU_Read_UserReg, &data, 1);
	return *(HTU2x_UserReg*)&data;
}

#endif
