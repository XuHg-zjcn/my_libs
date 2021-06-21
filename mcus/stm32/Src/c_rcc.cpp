/*
 * my_rcc.c
 *
 *  Created on: Apr 27, 2021
 *      Author: xrj
 */

#include "c_rcc.hpp"

#define APB1   (void*)0x40000000
#define APB2   (void*)0x40010000
#define AHB    (void*)0x40018000
#define Aend   (void*)0xA0001000

uint32_t PeriphAddr2Freq(void* addr)
{
	if(addr<APB1){
		return 0;                         //invalid periph addr
	}if(addr<APB2){
		return HAL_RCC_GetPCLK1Freq();    //APB1
	}if(addr<AHB){
		return HAL_RCC_GetPCLK2Freq();    //APB2
	}if(addr<Aend){
		return HAL_RCC_GetHCLKFreq();     //AHBx (x=1,2,3)
	}
	return 0;
}
