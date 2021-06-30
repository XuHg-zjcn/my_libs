#include "../Inc/c_pin.hpp"
#include "gpio.h"
#include "espressif/esp8266/pin_mux_register.h"


C_Pin::C_Pin(u8 pin_no)
{
    this->pin_no = pin_no;
}

void C_Pin::write_pin(PinState x)
{
    GPIO_OUTPUT(1<<pin_no, x);
}

PinState C_Pin::read_pin()
{
    return GPIO_INPUT_GET(pin_no);
}

u32 C_Pin::IO_MUX()
{
    return PERIPHS_IO_MUX + (u32)pin_no*0x40;
}

u8 C_Pin::IO_FUNC()
{
    const u8 func[16] = {
        FUNC_GPIO0, FUNC_GPIO1, FUNC_GPIO2, FUNC_GPIO3,
        FUNC_GPIO4, FUNC_GPIO5, FUNC_GPIO6, FUNC_GPIO7,
        FUNC_GPIO8, FUNC_GPIO9, FUNC_GPIO10, FUNC_GPIO11,
        FUNC_GPIO12, FUNC_GPIO13, FUNC_GPIO14, FUNC_GPIO15
    };
}