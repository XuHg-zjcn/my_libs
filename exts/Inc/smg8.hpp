/*
 * smg.hpp
 *
 *  Created on: 2021年6月2日
 *      Author: xrj
 */

#ifndef __SMG8_HPP__
#define __SMG8_HPP__

#include "mylibs_config.hpp"
#include "pins_manager.hpp"

//abstract class
class SMG8{
private:
	int n_digs;
public:
	SMG8(int n_digs);
	//需实现
	virtual void setDig(u32 i, u8 data)=0;
	virtual void off()=0;
	void setNum(u32 i, int num);
	void showNum(u32 i0, u32 i1, int num, u32 point, bool fill0);
	void showNum(int num1, u32 point, int num2);
	void showNum(int num, u32 point) {showNum(0, n_digs, num, point, false);}
	void showNum(int num)            {showNum(0, n_digs, num, 0, false);}
	void showNum(float num);
	void showNum(float num, u32 point);
	void showStr(char* str);
};

#endif /* __SMG8_HPP__ */
