/*
 * delay.c
 *
 *  Created on: 2021年4月16日
 *      Author: xrj
 */

#include "delay.hpp"
#include "mylibs_config.hpp"

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
	uint32_t n = 72*us/clk_loop;
	while(n--);
}
