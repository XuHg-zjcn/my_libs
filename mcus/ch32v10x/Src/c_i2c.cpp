/*
 * c_i2c.cpp
 *
 *  Created on: May 19, 2021
 *      Author: xrj
 */

#include "c_i2c.hpp"
#ifdef __C_I2C_HPP__

X_State C_I2C::Init(uint32_t Hz)
{
	I2C_InitTypeDef I2C_InitStruct;
	I2C_StructInit(&I2C_InitStruct);
	I2C_InitStruct.I2C_ClockSpeed = Hz;
	I2C_Init(this, &I2C_InitStruct);
	I2C_Cmd( this, ENABLE );
	return X_OK;
}

X_State C_I2C::Start()
{
	I2C_GenerateSTART(this, ENABLE);
	return X_OK;
}

X_State C_I2C::Stop()
{
	I2C_GenerateSTOP(this, ENABLE);
	return X_OK;
}

X_State C_I2C::SendAddr(u8 addr, CI2C_Direct tr)
{
	I2C_Send7bitAddress(this, addr<<1, tr);
	return X_OK;
}

X_State C_I2C::SendByte(u8 byte)
{
	I2C_SendData(this, byte);
	return X_OK;
}

uint8_t C_I2C::RecvByte()
{
	return I2C_ReceiveData(this);
}

X_State C_I2C::send(uint8_t DevAddr, uint8_t* pData, uint32_t Size)
{
	Start();
	SendAddr(DevAddr, CI2C_TX);
	while(Size){
		SendByte(*pData++);
		Size--;
	}
	Stop();
	return X_OK;
}

X_State C_I2C::recv(uint8_t DevAddr, uint8_t* pData, uint32_t Size)
{
	Start();
	SendAddr(DevAddr, CI2C_RX);
	while(Size){
		*pData++ = RecvByte();
	}
	Stop();
	return X_OK;
}

X_State C_I2C::WMem(uint8_t DevAddr, uint8_t MemAddr, uint8_t* pData, uint32_t Size)
{
	Start();
	SendAddr(DevAddr, CI2C_TX);
	SendByte(MemAddr);
	while(Size){
		SendByte(*pData++);
		Size--;
	}
	Stop();
	return X_OK;
}

X_State C_I2C::RMem(uint8_t DevAddr, uint8_t MemAddr, uint8_t* pData, uint32_t Size)
{
	Start();
	SendAddr(DevAddr, CI2C_TX);
	SendByte(MemAddr);
	Start();
	SendAddr(DevAddr, CI2C_RX);
	while(Size){
		SendByte(*pData++);
		Size--;
	}
	Stop();
	return X_OK;
}

/*
 * @param hi2c: C_I2C object pointer
 * @param addr: I2C device addr
 * @param mem_size: I2C_MEMADD_SIZE_8(16)BIT
 */
C_I2C_Dev::C_I2C_Dev(C_I2C *hi2c, uint16_t addr):
		hi2c(hi2c),DevAddr(addr<<1)
{
	ClockHz = 0;
	Timeout = 1000;
}

void C_I2C_Dev::set_Clock(uint32_t Hz)
{
	ClockHz = Hz;
}

void C_I2C_Dev::set_Timeout(uint32_t ms)
{
	Timeout = ms;
}

X_State C_I2C_Dev::send(uint8_t *data, uint32_t size)
{
	hi2c->send(DevAddr, data, size);
	return X_OK;
}

X_State C_I2C_Dev::recv(uint8_t* data, uint32_t size)
{
	hi2c->recv(DevAddr, data, size);
	return X_OK;
}

X_State C_I2C_Dev::Mem_write(uint16_t mem_addr, uint8_t *pData, uint16_t Size)
{
	hi2c->WMem(DevAddr, mem_addr, pData, Size);
	return X_OK;
}

X_State C_I2C_Dev::Mem_read(uint16_t mem_addr, uint8_t *pData, uint16_t Size)
{
	hi2c->RMem(DevAddr, mem_addr, pData, Size);
	return X_OK;
}
#endif
