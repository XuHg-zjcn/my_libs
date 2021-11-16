/*
 * co2_c8s.cpp
 *
 *  Created on: Nov 16, 2021
 *      Author: xrj
 */

#include "co2_c8s.hpp"

CO2_C8S::CO2_C8S(C_UART *uart):uart(uart){};

u16 CO2_C8S::wait_report()
{
	//TODO: 使用中断+地址识别(没有实现)
	while(true){
		if(uart->recv_byte(1000)==0x42){
			if(uart->recv_byte(2)==0x4D){
				break;
			}
		}
	}
	uart->recv(data, 14);
	return ((u16)data[4])<<8 | data[5];
}
