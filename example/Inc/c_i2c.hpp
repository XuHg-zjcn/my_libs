#include "c_stream.hpp"

typedef enum{
Master = 0,
Slave = 1
}

class C_I2C{
public:
    set_Clock(uint32_t Hz);
}

class C_I2C_Dev : public Stream{
private:
    uint16_t DevAddr;      //address of I2C device
    uint16_t MemAdd_size;  //memory addr size for mem write/mem read
    uint32_t ClockHz;      //reconfig I2C freq before each communication, 0: don't reconfig
    TransTypeStru trans;
public:
    C_I2C_Dev(uint16_t addr, uint16_t mem_size);
    set_Clock(uint32_t Hz);
    set_TransMode(TransTypeStru trans);
    Mem_write(uint16_t mem_addr, uint8_t *pData, uint16_t Size);
    Mem_read(uint16_t mem_addr, uint8_t *pData, uint16_t Size);
}
