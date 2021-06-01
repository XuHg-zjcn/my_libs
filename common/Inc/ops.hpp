/*
 * ops.h
 *
 *  Created on: Apr 26, 2021
 *      Author: xrj
 */

#ifndef INC_OPS_H_
#define INC_OPS_H_

#include "mylibs_config.hpp"


//a,b,c const, x,y,z vars

#define value_lower(x, a)   (x<a)?a:x
#define value_upper(x, b)   (x>b)?b:x
#define value_clip(x, a, b) (x<a)?a:(x>b?b:x)

#define mean(s, n)      (s/n)
#define std(s, s2, n)   ((s2 - s1*s1/n)/n)

template<typename T>
uint32_t sum(T* p, uint32_t n)
{
	uint32_t ret=0;
	for(uint32_t i=0;i<n;i++){
		ret += *p++;
	}
	return ret;
}

//for 12bit ADC sample, n should <= 256, else maybe overflow
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

u32 u32_sqrt(u32 x);
u32 u64_sqrt(u64 x);

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
	u32 nstd = u64_sqrt(s2*len - (u64)s*s);
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

#endif /* INC_OPS_H_ */
