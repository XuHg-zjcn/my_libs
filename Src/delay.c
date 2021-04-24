/*
 * delay.c
 *
 *  Created on: 2021年4月16日
 *      Author: xrj
 */

#include "stm32f4xx_hal.h"


void Delay_loopN(uint32_t n)
{
	while(n--);
}

void Delay_clocks(uint32_t n)
{
	n = n/7 - 3;
	while(n--);
}

void Delay_us(uint32_t us)
{
	uint32_t n = ((uint64_t)HAL_RCC_GetSysClockFreq())*us/7000000 - 40;
	while(n--);
}
