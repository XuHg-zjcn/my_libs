/*
 * tm1650.cpp
 *
 *  Created on: Jul 1, 2021
 *      Author: xrj
 */

#include "tm1650.hpp"
#include "font_smg8.h"


TM1650::TM1650(TM_I2C *i2c):SMG8(4)
{
	this->i2c = i2c;
}

void TM1650::setDig(u32 i, u8 data)
{
	if(i>4){
		return;
	}
	i2c->Start();
	i2c->send_byte(0x68+i*2);
	i2c->send_byte(data);
	i2c->Stop();
}

u8 TM1650::readKey(u32 abc, u32 dig)
{
	u8 ret;
	if(abc > 6 || dig > 3){
		return 0;
	}
	i2c->Start();
	i2c->send_byte(0x44+0x8*abc+dig);
	ret = i2c->recv_byte(true);
	i2c->Stop();
	return ret;
}

void TM1650::send_commad(TM1650_Commands cmd)
{
	i2c->Start();
	i2c->send_byte(0x48);
	i2c->send_byte(*(u8*)&cmd);
	i2c->Stop();
}

void TM1650::off()
{
	send_commad(TM1650_Commands(1, false, false));
}
