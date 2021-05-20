#include "c_stream.hpp"

typedef enum{
    Master = 0,
    Slave = 1
};

class C_I2C{
public:
    X_State set_Clock(uint32_t Hz);
};

class C_I2C_Dev : public Stream{
private:
    C_I2C *hi2c;
    uint16_t DevAddr;      //address of I2C device
    uint16_t MemAdd_size;  //memory addr size for mem write/mem read
    uint32_t ClockHz;      //reconfig I2C freq before each communication, 0: don't reconfig
    uint32_t Timeout;
    TransTypeStru trans;
public:
    C_I2C_Dev(uint16_t addr, uint16_t mem_size);
    void set_Clock(uint32_t Hz);
    void set_Timeout(uint32_t ms);
    X_State set_TransMode(TransTypeStru trans);
    X_State Mem_write(uint16_t mem_addr, uint8_t *pData, uint16_t Size);
    X_State Mem_read(uint16_t mem_addr, uint8_t *pData, uint16_t Size);
};
