#ifndef __S_UART_HPP
#define __S_UART_HPP

#include "c_pin.hpp"

class S_UART{
private:
    C_Pin tx;
    C_Pin rx;
    uint32_t us;
public:
    S_UART(C_Pin tx, C_Pin rx):tx(tx),rx(rx){};
    S_UART(C_Pin tx, C_Pin rx, uint32_t bps):tx(tx),rx(rx),us(1000000/bps){};
    void set_intt();
    void send_byte(u8 byte);
    void send_data(u8* data, u32 len);
    u8 recv_byte();
    void recv_block(u8* data, u32 len);
    void recv_it(u8* data, u32 len);
};

#endif
