/*
 * ops.c
 *
 *  Created on: 2021年6月1日
 *      Author: xrj
 */


#include "ops.hpp"

//return : x*a/b
u32 xfact(u32 x, u32 a, u32 b)
{
	u32 ret = x*(a/b);
	a %= b;
	return ret+(x/b)*a;
}
