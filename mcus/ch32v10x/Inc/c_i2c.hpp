/*
 * c_i2c.hpp
 *
 *  Created on: May 19, 2021
 *      Author: xrj
 */

#include "ch32v10x_i2c.h"
#if !defined(__C_I2C_HPP__) && \
	defined(__CH32V10x_I2C_H)
#define __C_I2C_HPP__

#include "c_stream.hpp"

#include "x_base.hpp"

typedef enum{
    CI2C_TX = I2C_Direction_Transmitter,
    CI2C_RX = I2C_Direction_Receiver,
}CI2C_Direct;

class C_I2C : public I2C_TypeDef{
protected:
    X_State Start();
    X_State Stop();
    X_State SendAddr(u8 addr, CI2C_Direct tr);
    X_State SendByte(u8 byte);
    uint8_t RecvByte();
public:
    X_State Init();
    X_State Init(uint32_t Hz);
	X_State set_Clock(uint32_t Hz);
    X_State send(uint8_t DevAddr, uint8_t* pData, uint32_t Size);
    X_State recv(uint8_t DevAddr, uint8_t* pData, uint32_t Size);
    X_State WMem(uint8_t DevAddr, uint8_t MemAddr, uint8_t* pData, uint32_t Size);
    X_State RMem(uint8_t DevAddr, uint8_t MemAddr, uint8_t* pData, uint32_t Size);
};


#ifdef USE_ABSTRACT
class C_I2C_Dev : public Stream{
#else
class C_I2C_Dev{
#endif
private:
	C_I2C *hi2c;
	const uint16_t DevAddr;      //address of I2C device
    uint32_t ClockHz;      //reconfig I2C freq before each communication, 0: don't reconfig
    //TransTypeStru trans;
    uint32_t Timeout;
public:
    C_I2C_Dev(C_I2C *hi2c, uint16_t addr);
    void set_Clock(uint32_t Hz);
    void set_Timeout(uint32_t ms);
    //void set_TransMode(TransTypeStru trans);
    X_State send(uint8_t* data, uint32_t size);
    X_State recv(uint8_t* data, uint32_t size);
    X_State Mem_write(uint16_t mem_addr, uint8_t *pData, uint16_t Size);
    X_State Mem_read(uint16_t mem_addr, uint8_t *pData, uint16_t Size);
};

#endif /* __C_I2C_HPP__ */
