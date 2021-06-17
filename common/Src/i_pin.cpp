/*
 * i_gpio.cpp
 *
 *  Created on: 2021年6月13日
 *      Author: xrj
 */

#include "../Inc/i_pin.hpp"


void I_Pin::toggle_pin()
{
	write_pin(!read_pin());
}

//blocking until read_pin() == state
void I_Pin::wait_pin(PinState state)
{
	while(read_pin() xor state);
}

u32 I_Pin::wait_timeout(PinState state, u32 timeout)
{
	while((read_pin() xor state) and timeout){
		timeout--;
	}
	return timeout;
}

//阻塞式测量
u32 I_Pin::wait_count(PinState state, u32 m, u32 M)
{
	u32 n=0;
	state = !state;
	while((read_pin() xor state) and n<M or n<m){
		n++;
	}
	return n;
}
