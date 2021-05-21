/*
 * c_i2c.cpp
 *
 *  Created on: May 19, 2021
 *      Author: xrj
 */

#include "c_i2c.hpp"

X_State C_I2C::set_Clock(uint32_t Hz)
{
	this->Init.ClockSpeed = Hz;
	return (X_State)HAL_I2C_Init(this);
}

/*
 * @param hi2c: C_I2C object pointer
 * @param addr: I2C device addr
 * @param mem_size: I2C_MEMADD_SIZE_8(16)BIT
 */
C_I2C_Dev::C_I2C_Dev(C_I2C *hi2c, uint16_t addr, uint16_t mem_size):
		hi2c(hi2c),DevAddr(addr),MemAdd_size(mem_size)
{
	this->ClockHz = 0;
	this->Timeout = 1000;
}

void C_I2C_Dev::set_Clock(uint32_t Hz)
{
	this->ClockHz = Hz;
}

void C_I2C_Dev::set_Timeout(uint32_t ms)
{
	this->Timeout = ms;
}

void C_I2C_Dev::set_TransMode(TransTypeStru trans)
{
	this->trans = trans;
}

X_State C_I2C_Dev::send(uint8_t *data, uint32_t size)
{
	if(trans.DMA_use){
		HAL_I2C_Master_Transmit_DMA(hi2c, DevAddr, data, size);
	}else if(trans.trans == TransBlocking){
		HAL_I2C_Master_Transmit(hi2c, DevAddr, data, size, Timeout);
	}else{
		HAL_I2C_Master_Transmit_IT(hi2c, DevAddr, data, size);
	}
	return X_OK;
}

X_State C_I2C_Dev::recv(uint8_t* data, uint32_t size)
{
	if(trans.DMA_use){
		HAL_I2C_Master_Receive_DMA(hi2c, DevAddr, data, size);
	}else if(trans.trans == TransBlocking){
		HAL_I2C_Master_Receive(hi2c, DevAddr, data, size, Timeout);
	}else{
		HAL_I2C_Master_Receive_IT(hi2c, DevAddr, data, size);
	}
	return X_OK;
}

X_State C_I2C_Dev::Mem_write(uint16_t mem_addr, uint8_t *pData, uint16_t Size)
{
	if(trans.DMA_use){
		HAL_I2C_Mem_Write_DMA(hi2c, DevAddr, mem_addr, MemAdd_size, pData, Size);
	}else if(trans.trans == TransBlocking){
		HAL_I2C_Mem_Write(hi2c, DevAddr, mem_addr, MemAdd_size, pData, Size, Timeout);
	}else{
		HAL_I2C_Mem_Write_IT(hi2c, DevAddr, mem_addr, MemAdd_size, pData, Size);
	}
	return X_OK;
}

X_State C_I2C_Dev::Mem_read(uint16_t mem_addr, uint8_t *pData, uint16_t Size)
{
	if(trans.DMA_use){
		HAL_I2C_Mem_Read_DMA(hi2c, DevAddr, mem_addr, MemAdd_size, pData, Size);
	}else if(trans.trans == TransBlocking){
		HAL_I2C_Mem_Read(hi2c, DevAddr, mem_addr, MemAdd_size, pData, Size, Timeout);
	}else{
		HAL_I2C_Mem_Read_IT(hi2c, DevAddr, mem_addr, MemAdd_size, pData, Size);
	}
	return X_OK;
}
