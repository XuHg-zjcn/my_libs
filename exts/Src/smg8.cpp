/*
 * smg.cpp
 *
 *  Created on: 2021年6月2日
 *      Author: xrj
 */

#include "smg8.hpp"
#ifdef __SMG8_HPP__

#include "font_smg8.h"
#include "myints.h"
#include <stdlib.h>


SMG8::SMG8(int n_digs)
{
	this->n_digs = n_digs;
}

void SMG8::setNum(u32 i, int num)
{
	if(num>9){
		return;
	}
	setDig(i, fontsmg8[num]);
}

/*
 * @param i0: index of start
 * @param i1: index of end+1
 * @param num: number to show
 * @param point: <0:don't show, >=0:point at (n-1)th number
 * @param fill0: is fill 0 at head?
 */
void SMG8::showNum(u32 i0, u32 i1, int num, u32 point, bool fill0)
{
	u32 i=i1-i0;
	u32 ip = i0+point;
	if(num<0){
		i-=1;
	}
	u32 m=1;
	for(;i>0;i--){
		m*=10;
	}//m = max abs number can show
	if(abs(num)>=m){
		return;
	}
	i1--;
	do{
		setDig(i1 ,fontsmg8[num%10]|(i1==ip)<<7);
		i1--;
		num/=10;
	}while(num!=0);
	u8 fill = fill0 ? fontsmg8[0] : 0;
	while(i1 < n_digs){
		setDig(i1, fill|(i1==ip)<<7);
		i1--;
	}
}

void SMG8::showNum(int num1, u32 point, int num2)
{
	showNum(0, point, num1, point, false);
	showNum(point, n_digs, num2, 0, true);
}

void SMG8::showNum(float num)
{
	int32_t tmp=flog10(num)+1;
	if(tmp<0){
		tmp=1;
	}else if(tmp>(n_digs-(num<0))){
		return; //太大
	}
	showNum(num, tmp);
}

void SMG8::showNum(float num, u32 point)
{
	u32 mul = 1;
	for(int i=point;i<4;i++){
		mul*=10;
	}
	int n2=(int)num;
	showNum(n2, point, (int)((num-n2)*mul));
}

#endif
