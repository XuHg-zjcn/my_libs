#ifndef C_PIN_HPP
#define C_PIN_HPP

#include "common/Inc/i_pin.hpp"

class C_Pin : public I_Pin{
private:
    u8 pin_no;
public:
    C_Pin(u8 pin_no);
    void write_pin(PinState x);
    PinState read_pin();
    inline u8 get_pin_no() {return pin_no;}
    u32 IO_MUX();
    u32 IO_FUNC();
};

#endif