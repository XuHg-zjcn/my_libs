/*
 * delay.c
 *
 *  Created on: 2021年4月16日
 *      Author: xrj
 */

#include "delay.h"

#include "stm32f4xx_hal.h"

#define clk_loop 7
#define clk_call 3
#define clk_getFreq 37

void Delay_loopN(uint32_t n)
{
	while(n--);
}

void Delay_clocks(uint32_t n)
{
	n = n/clk_loop - clk_call;
	while(n--);
}

void Delay_us(uint32_t us)
{
	uint32_t n = ((uint64_t)HAL_RCC_GetSysClockFreq())\
			     *us/1000000/clk_loop - clk_getFreq - clk_call;
	while(n--);
}
