#include "esp8266/Inc/c_i2c.hpp"
#include "i2c_master.h"

void C_I2C::send_byte(u8 data)
{
    i2c_master_writeByte(data);
}

u8 C_I2C::recv_byte()
{
    return i2c_master_readByte();
}

void C_I2C::send(u8* p, u32 n)
{
    for(int i=0;i<n;i++){
        send_byte(*p++);
    }
}

void C_I2C::recv(u8* p, u32 n)
{
    for(int i=0;i<n;i++){
        *p++ = recv_byte();
    }
}

C_I2C_Dev::C_I2C_Dev(C_I2C *ci2c, u8 dev_addr)
{
    this->ci2c = ci2c;
    this->dev_addr = dev_addr;
}

void C_I2C_Dev::Mem_write(u8 mem_addr, u8* p, u32 n)
{
    ci2c->send_byte(mem_addr);
    ci2c->send(p, n);
}

void C_I2C_Dev::Mem_read(u8 mem_addr, u8* p, u32 n)
{
    ci2c->send_byte(mem_addr);
    ci2c->recv(p, n);
}
