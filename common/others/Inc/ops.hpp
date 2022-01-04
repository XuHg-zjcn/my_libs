/*
 * ops.h
 *
 *  Created on: Apr 26, 2021
 *      Author: xrj
 */

#ifndef __OPS_H__
#define __OPS_H__

#include "mylibs_config.hpp"
#include "myints.h"

#if !defined(STM32F1) && !defined(STM32F4)

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))

#endif

//a,b,c const, x,y,z vars

#define value_lower(x, a)   (x<a)?a:x
#define value_upper(x, b)   (x>b)?b:x
#define value_clip(x, a, b) ((x<a)?a:(x>b?b:x))

#define mean(s, n)      (s/n)
#define std(s, s2, n)   ((s2 - s1*s1/n)/n)

template<typename T>
u32 uint_sqrt(T x)
{
	if(x == 0){ //CLZ undefined at 0
		return 0;
	}
	u32 bits = sizeof(T)*8 - __builtin_clzll(x);
	u32 ret = 0;
	i32 i=(bits-1)/2;
	for(;i>=0;i--){
		u32 tmp = ret|(1<<i);
		T x2 = (T)tmp*(T)tmp;
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

template<typename T>
uint32_t sum(T* p, uint32_t n)
{
	uint32_t ret=0;
	for(uint32_t i=0;i<n;i++){
		ret += *p++;
	}
	return ret;
}

/*
 * for 12bit ADC sample, else maybe overflow
 * To=u32, n should <= 256
 * To=u64, n can be 2^40
 */
template<typename T, typename To>
To sum2(T* p, uint32_t n)
{
	To ret=0;
	for(uint32_t i=0;i<n;i++){
		ret += (*p)*(*p);
		p++;
	}
	return ret;
}

//var>=0, so can use unsigned long long
template<typename T>
u64 n2var(T* p, uint32_t n)
{
	u64 s = sum(p, n);
	return sum2<T, u64>(p, n)*n - s*s;
}

template<typename T>
u32 istd(T* p, uint32_t n)
{
	return uint_sqrt<u64>(n2var(p, n))/n;
}

//for 12bit ADC sample, n should < 2^10.5 = 1448, else maybe overflow
template<typename T>
uint32_t isum(T* p, uint32_t n)
{
	uint32_t ret=0;
	for(uint32_t i=0;i<n;i++){
		ret += (*p++)*i;
	}
	return ret;
}

//自相关，需要len+n个数据
template<typename T>
void zxg(T* p, u32 len, u32 *res, u32 n)
{
	for(u32 i=0;i<len;i++){
		T *p2 = p;
		for(u32 j=0;j<n;j++){
			res[j] += (*p) * (*p2++);
		}
		p++;
	}
}

template<typename T>
void linear_fill(T* p, u32 len, T a, T b)
{
	u32 A=a,B=b;
	len--;
	for(u32 i=0;i<=len;i++){
		*p++ = (B*i + A*(len-i))/len;
	}
}

template<typename T>
void filter(T* p, u32 len, u32 xstd100)
{
	u32 s = sum(p, len);
	u64 s2 = sum2<T, u64>(p, len);
	u32 nstd = uint_sqrt(s2*len - (u64)s*s);
	u32 max = (s*100+nstd*xstd100)/(len*100);
	u32 min = (s*100-nstd*xstd100)/(len*100);
	p++;
	for(u32 i=1;i<len;i++){
		if(*p > max){
			T* p0=p-1;
			while(*p>max && i<len){i++;p++;};
			linear_fill(p0, p-p0, *p0, *p);
		}else if(*p < min){
			T* p0=p-1;
			while(*p<min && i<len){i++;p++;};
			linear_fill(p0, p-p0, *p0, *p);
		}else{
			i++;
			p++;
		}
	}
}

//每组n个数据，共m组，需要n*m个数据，每组求和，向右移shift位并写入原位（每组的和对应一个原来数据位置）
template<typename T>
void mean_zip(T* p, u32 n, u32 m, u32 shift)
{
	T* p2 = p;
	for(u32 i=0;i<m;i++){
		u32 sum=0;
		for(u32 j=0;j<n;j++){
			sum += *p++;
		}
		*p2++ = sum>>shift;
	}
}

u32 xfact(u32 x, u32 a, u32 b);

#endif /* __OPS_H__ */
