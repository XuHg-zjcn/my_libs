#include "s_uart.hpp"
#include "delay.hpp"

#if defined(__S_UART_HPP__) && \
	defined(__DELAY_HPP__)

void S_UART::send_byte(u8 byte)
{
    tx.write_pin(Pin_Reset);
    XDelayUs(us);
    for(uint8_t i=0;i<8;i++){
        tx.write_pin((PinState)(byte&0x01));
        XDelayUs(us);
    }
    tx.write_pin(Pin_Set);
    XDelayUs(us);
}

void S_UART::send_data(u8* data, u32 len)
{
    while(len--){
        send_byte(*data++);
    }
}

u8 S_UART::recv_byte()
{
    u8 ret=0;
    rx.wait_pin(Pin_Reset);
    XDelayUs(us);
    for(uint8_t i=0;i<8;i++){
        ret<<=1;
        ret|=rx.read_pin();
    }
    return ret;
}

void S_UART::recv_block(u8* data, u32 len)
{
    while(len--){
        *data++ = recv_byte();
    }
}

#endif
