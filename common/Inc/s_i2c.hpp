/*
 * s_i2c.hpp
 *
 *  Created on: 2021年7月2日
 *      Author: xrj
 */

#ifndef COMMON_INC_S_I2C_HPP_
#define COMMON_INC_S_I2C_HPP_

#include "myints.h"
#include "c_pin.hpp"

#define SCL_TIMEOUT 10000

class S_I2C_Dev;

class S_I2C{
private:
	C_Pin scl;
	C_Pin sda;
	u32 loops;
	//TODO: lock
public:
	X_State Start();
	void Stop();
	X_State recv_ack();
	void send_ack(bool nack);
	X_State send_addr(u8 addr, bool RW);
	X_State send_byte(u8 byte);
	u8 recv_byte(bool nack);
	S_I2C(C_Pin scl, C_Pin sda);
	void set_clock(u32 Hz);
	void send(u8 addr, u8* data, u32 len);
	void recv(u8 addr, u8* data, u32 len);
	//void scan(u8 begin, u8 end);
	//void scan();
};

class S_I2C_Dev{
private:
	S_I2C* i2c;
	uint8_t DevAddr;
public:
	S_I2C_Dev(S_I2C* i2c, u8 addr);
	void send(u8* data, u32 len);
	void recv(u8* data, u32 len);
	void Mem_write(u8 mem, u8* data, u32 len);
	void Mem_read(u8 mem, u8* data, u32 len);
};

#endif /* COMMON_INC_S_I2C_HPP_ */
