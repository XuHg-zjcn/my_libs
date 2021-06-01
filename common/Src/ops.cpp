/*
 * ops.c
 *
 *  Created on: 2021年6月1日
 *      Author: xrj
 */


#include <ops.hpp>

u32 u32_sqrt(u32 x)
{
	if(x == 0){ //CLZ undefine at 0
		return 0;
	}
	u32 bits = 32 - __builtin_clz(x);
	u32 ret = 0;
	for(i32 i=(bits-1)/2;i>=0;i--){
		u32 tmp = ret|(1<<i);
		u32 x2 = tmp*tmp;
		if(x2 == x){
			return tmp;
		}else if(x2 < x){
			ret = tmp;
		}/*else if(x2 > x){
			continue;
		}*/
	}
	return ret;
}

u32 u64_sqrt(u64 x)
{
	if(x == 0){ //CLZ undefined at 0
		return 0;
	}
	u32 bits = 64 - __builtin_clzll(x);
	u32 ret = 0;
	for(i32 i=(bits-1)/2;i>=0;i--){
		u32 tmp = ret|(1<<i);
		u64 x2 = (u64)tmp*tmp;
		if(x2 == x){
			return tmp;
		}else if(x2 < x){
			ret = tmp;
		}/*else if(x2 > x){
			continue;
		}*/
	}
	return ret;
}

//return : x*a/b
u32 xfact(u32 x, u32 a, u32 b)
{
	u32 ret = x*(a/b);
	a %= b;
	return ret+(x/b)*a;
}
