#ifndef C_I2C_HPP
#define C_I2C_HPP

#include "mylibs_config.hpp"
#include "common/Inc/myints.h"
#include "i2c_master.h"

class C_I2C{
public:
    void Init();
    void send_byte(u8 data);
    u8   recv_byte();
    void send(u8* p, u32 n);
    void recv(u8* p, u32 n);
};

class C_I2C_Dev{
private:
    C_I2C *ci2c;
    u8 dev_addr;
public:
    C_I2C_Dev(C_I2C *ci2c, u8 dev_addr);
    void Mem_write(u8 mem_addr, u8* p, u32 n);
    void Mem_read(u8 mem_addr, u8* p, u32 n);
};

#endif
