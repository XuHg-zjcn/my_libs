/*
 * delay.c
 *
 *  Created on: 2021年4月16日
 *      Author: xrj
 */

#include "delay.hpp"
#include "mylibs_config.hpp"

#ifdef __STM32F1xx_HAL_H
#define clk_loop 13
#define clk_call 4
#define clk_getFreq 50
#endif
#ifdef __STM32F4xx HAL_H
#define clk_loop 7
#define clk_call 3
#define clk_getFreq 37
#endif

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
