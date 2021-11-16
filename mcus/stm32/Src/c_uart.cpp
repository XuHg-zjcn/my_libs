/*
 * c_uart.cpp
 *
 *  Created on: Nov 16, 2021
 *      Author: xrj
 */

#include "c_uart.hpp"

#define Timeout 1000

X_State C_UART::send(u8* pData, u32 Size)
{
	return (X_State)HAL_UART_Transmit(this, pData, Size, Timeout);
}

X_State C_UART::recv(u8* pData, u32 Size)
{
	return (X_State)HAL_UART_Receive(this, pData, Size, Timeout);
}

i16 C_UART::recv_byte(u32 timeout)
{
	u32 Tickstart = GetTick();
	while(__HAL_UART_GET_FLAG(this, UART_FLAG_RXNE) == RESET){
		if(GetTick()-Tickstart>timeout){
			return -1;
		}
	}
	return (uint8_t)(this->Instance->DR) & 0xff;
}

void C_UART_Ex::ISR_func(u8 byte)
{
	if(GetTick()-last_ts<min_tick){
		return;
	}
	//TODO: 实现地址识别
}
