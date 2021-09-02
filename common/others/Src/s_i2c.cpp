/*
 * s_i2c.cpp
 *
 *  Created on: 2021年7月2日
 *      Author: xrj
 */

#include "s_i2c.hpp"
#include "delay.hpp"

#define LOW(x)  x.loadCfg(Pin_OD0)
#define HIGH(x) x.loadCfg(Pin_InUp)
#define READ(x) x.read_pin()
#define SI2C_DELAY(x) XDelayUs(x)

#define I2C_WRITE false
#define I2C_READ  true

S_I2C::S_I2C(C_Pin scl, C_Pin sda):scl(scl),sda(sda)
{
	HIGH(scl);
	HIGH(sda);
	set_clock(50000);
}

void S_I2C::set_clock(u32 Hz)
{
	u32 div = 1000000/Hz;
	if(div!=0){
		loops=div;
	}
}

X_State S_I2C::Start()
{
	HIGH(sda);
	HIGH(scl);
	if(READ(sda) == Pin_Reset){
		return X_Busy;
	}
	LOW(sda);
	SI2C_DELAY(loops);
	LOW(scl);
	SI2C_DELAY(loops);
	return X_OK;
}

void S_I2C::Stop()
{
	HIGH(scl);
	SI2C_DELAY(loops);
	HIGH(sda);
	SI2C_DELAY(loops);
}

X_State S_I2C::recv_ack()
{
	HIGH(sda);
	SI2C_DELAY(loops);
	HIGH(scl);
	SI2C_DELAY(loops);
	X_State ret = sda.read_pin()?X_Error:X_OK;
	SI2C_DELAY(loops);
	LOW(scl);
	SI2C_DELAY(loops);
	return ret;
}

void S_I2C::send_ack(bool nack)
{
	nack?HIGH(sda):LOW(sda);
	SI2C_DELAY(loops);
	HIGH(scl);
	SI2C_DELAY(loops*2);
	LOW(scl);
	SI2C_DELAY(loops);
	HIGH(sda);
}

/*
 * 总线仲裁
 * @param RW: 0写 1度
 *
 */
X_State S_I2C::send_addr(u8 addr, bool RW)
{
	addr<<=1;
	addr|=RW;
	for(int i=0;i<8;i++){
		addr&0x80?HIGH(sda):LOW(sda);
		addr<<=1;
		SI2C_DELAY(loops);
		HIGH(scl);
		/*if(addr&0x80){
			if(sda.wait_timeout(Pin_Reset, loops/5)!=0){
				return X_Busy;
			}
		}else{*/
			SI2C_DELAY(loops);
		//}
		LOW(scl);
		SI2C_DELAY(loops);
	}
	return recv_ack();
}

X_State S_I2C::send_byte(u8 byte)
{
	for(int i=0;i<8;i++){
		byte&0x80?HIGH(sda):LOW(sda);
		byte<<=1;
		SI2C_DELAY(loops);
		HIGH(scl);
		SI2C_DELAY(loops*2);
		LOW(scl);
		SI2C_DELAY(loops);
	}
	return recv_ack();
}

u8 S_I2C::recv_byte(bool nack)
{
	u8 ret=0;
    HIGH(sda);
	for(int i=0;i<8;i++){
		ret<<=1;
		SI2C_DELAY(loops);
		HIGH(scl);
		SI2C_DELAY(loops);
		ret|=READ(sda);
		SI2C_DELAY(loops);
		LOW(scl);
		SI2C_DELAY(loops);
	}
	send_ack(nack);
	SI2C_DELAY(loops);
	HIGH(sda);
	SI2C_DELAY(loops);
	return ret;
}

void S_I2C::send(u8 addr, u8* data, u32 len)
{
	Start();
	send_addr(addr, I2C_WRITE);
	for(u32 i=0;i<len;i++){
		send_byte(*data++);
	}
	Stop();
}

void S_I2C::recv(u8 addr, u8* data, u32 len)
{
	Start();
	send_addr(addr, I2C_READ);
	for(;len>0;len--){
		*data++ = recv_byte(len!=0);
	}
	Stop();
}

S_I2C_Dev::S_I2C_Dev(S_I2C* i2c, u8 addr)
{
	this->i2c = i2c;
	this->DevAddr = addr;
}

void S_I2C_Dev::send(u8* data, u32 len)
{
	i2c->send(DevAddr, data, len);
}

void S_I2C_Dev::recv(u8* data, u32 len)
{
	i2c->recv(DevAddr, data, len);
}

void S_I2C_Dev::Mem_write(u8 mem, u8* data, u32 len)
{
	if(i2c->Start()!=X_OK){goto stop;};
	if(i2c->send_addr(DevAddr, I2C_WRITE)!=X_OK){goto stop;}
	if(i2c->send_byte(mem)!=X_OK){goto stop;};
	for(u32 i=0;i<len;i++){
		i2c->send_byte(*data++);
	}
	stop:i2c->Stop();
}

void S_I2C_Dev::Mem_read(u8 mem, u8* data, u32 len)
{
	if(i2c->Start()!=X_OK){goto stop;};
	if(i2c->send_addr(DevAddr, I2C_WRITE)!=X_OK){goto stop;}
	if(i2c->send_byte(mem)!=X_OK){goto stop;}
	if(i2c->Start()!=X_OK){goto stop;}
	if(i2c->send_addr(DevAddr, I2C_READ)!=X_OK){goto stop;}
	for(;len>0;len--){
		*data++ = i2c->recv_byte(len==1);
	}
	stop:i2c->Stop();
}
