/*
 * myints.c
 *
 *  Created on: 2021年6月8日
 *      Author: xrj
 */

#include "myints.h"

int sign(float x)
{
	if(x==0){
		return 0;
	}
	return F32_SIGN(x)?-1:1;
}

int flog10(float x)
{
#define N_MIN 39
#define N_MAX 39
	const u32 tab[] = {
	0x0ae398, 0x6ce3ee, 0x081cea, 0x2a2425, 0x54ad2e, 0x04ec3d, 0x26274c, 0x4fb11f, 0x01ceb3,           //-39 ... -31
	0x224260, 0x4ad2f8, 0x7d87b6, 0x1e74d2, 0x461206, 0x779688, 0x1abe15, 0x416d9a, 0x71c901, 0x171da0, //-30 ... -21
	0x3ce508, 0x6c1e4a, 0x1392ef, 0x3877aa, 0x669595, 0x101d7d, 0x3424dc, 0x612e13, 0x0cbccc, 0x2febff, //-20 ... -11
	0x5be6ff, 0x09705f, 0x2bcc77, 0x56bf95, 0x0637bd, 0x27c5ac, 0x51b717, 0x03126f, 0x23d70a, 0x4ccccd, //-10 ...  -1
	0x000000, 0x200000, 0x480000, 0x7a0000, 0x1c4000, 0x435000, 0x742400, 0x189680, 0x3ebc20, 0x6e6b28, //  0 ...   9
	0x1502f9, 0x3a43b7, 0x68d4a5, 0x1184e7, 0x35e621, 0x635fa9, 0x0e1bca, 0x31a2bc, 0x5e0b6b, 0x0ac723, // 10 ...  19
	0x2d78ec, 0x58d727, 0x078678, 0x296816, 0x53c21c, 0x045951, 0x256fa6, 0x4ecb8f, 0x013f39, 0x218f08, // 20 ...  29
	0x49f2ca, 0x7c6f7c, 0x1dc5ae, 0x453719, 0x7684df, 0x1a130c, 0x4097ce, 0x70bdc2, 0x167699};          // 30 ...  38
	int exp8=F32_EXP8(x);
	int lg10=FLOOR_DIV(exp8*3, 10);
	//判断是否会偏小1
	if((exp8+1)*3/10>lg10 && F32_MANT(x) >= tab[lg10+N_MIN+1]){
		lg10++;
	}
	return lg10;
}
