/*
 * s_i2c.cpp
 *
 *  Created on: 2021年7月2日
 *      Author: xrj
 */

#include "s_i2c.hpp"
#include "delay.hpp"

#define LOW(x)  x.loadXCfg(GPIO_GP_OD0 & Out_2MHz)
#define HIGH(x) x.loadXCfg(GPIO_In_Up)
#define READ(x) x.read_pin()

#define I2C_WRITE false
#define I2C_READ  true

S_I2C::S_I2C(C_Pin scl, C_Pin sda):scl(scl),sda(sda)
{
	scl.loadXCfg(GPIO_In_Up);
	sda.loadXCfg(GPIO_In_Up);
	set_clock(50000);
}

void S_I2C::set_clock(u32 Hz)
{
	u32 sys = HAL_RCC_GetSysClockFreq();
	u32 div = (sys*2)/(Hz*3*clk_loop);
	if(div!=0){
		loops=div;
	}
}

X_State S_I2C::Start()
{
	if(READ(sda) == Pin_Reset){
		return X_Busy;
	}
	LOW(sda);
	Delay_loopN(loops);
	LOW(scl);
	Delay_loopN(loops);
	return X_OK;
}

void S_I2C::Stop()
{
	HIGH(scl);
	Delay_loopN(loops);
	HIGH(sda);
	Delay_loopN(loops);
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
		Delay_loopN(loops);
		HIGH(scl);
		/*if(addr&0x80){
			if(sda.wait_timeout(Pin_Reset, loops/5)!=0){
				return X_Busy;
			}
		}else{*/
			Delay_loopN(loops);
		//}
		LOW(scl);
		Delay_loopN(loops);
	}
	HIGH(sda);
	Delay_loopN(1);
	return sda.wait_timeout(Pin_Reset, loops/2)==0?X_Timeout:X_OK;
}

X_State S_I2C::send_byte(u8 byte)
{
	if(scl.wait_timeout(Pin_Set, SCL_TIMEOUT)==0){
		return X_Timeout;
	}
	for(int i=0;i<8;i++){
		byte&0x80?HIGH(sda):LOW(sda);
		byte<<=1;
		Delay_loopN(loops);
		HIGH(scl);
		Delay_loopN(loops);
		LOW(scl);
		Delay_loopN(loops);
	}
	HIGH(sda);
	return sda.wait_timeout(Pin_Reset, loops/2)==0?X_Timeout:X_OK;
}

u8 S_I2C::recv_byte()
{
	u8 ret;
	if(scl.wait_timeout(Pin_Set, SCL_TIMEOUT)==0){
		return X_Timeout;
	}
	for(int i=0;i<8;i++){
		HIGH(scl);
		Delay_loopN(loops);
		ret|=READ(sda);
		Delay_loopN(loops);
		LOW(scl);
		Delay_loopN(loops);
		ret<<=1;
	}
	LOW(sda);
	Delay_loopN(loops);
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
	for(u32 i=0;i<len;i++){
		*data++ = recv_byte();
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
	i2c->Start();
	i2c->send_addr(DevAddr, I2C_READ);
	i2c->send_byte(mem);
	for(u32 i=0;i<len;i++){
		i2c->send_byte(*data++);
	}
	i2c->Stop();
}

void S_I2C_Dev::Mem_read(u8 mem, u8* data, u32 len)
{
	i2c->Start();
	i2c->send_addr(DevAddr, I2C_WRITE);
	i2c->send_byte(mem);
	for(u32 i=0;i<len;i++){
		*data++ = i2c->recv_byte();
	}
	i2c->Stop();
}
