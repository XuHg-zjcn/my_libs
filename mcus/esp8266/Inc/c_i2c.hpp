#ifndef C_I2C_HPP
#define C_I2C_HPP

#include "mylibs_config.hpp"
#include "myints.h"
#include "driver/i2c.h"

//#define CI2C_ENABLE_NOSS //允许使用不发送Start Stop的底层接口

class C_I2C_Dev;

class C_I2C{
private:
    i2c_cmd_handle_t cmd;
public:
    C_I2C();
    void Init();
#ifdef CI2C_ENABLE_NOSS
    void start();
    void stop();
    bool send_byte_noss(u8 data);
    void recv_byte_noss(u8 data, bool ack_en);
    void send_noss(u8* p, u32 n);
    void recv_noss(u8* p, u32 n);
#endif
    friend C_I2C_Dev;
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
