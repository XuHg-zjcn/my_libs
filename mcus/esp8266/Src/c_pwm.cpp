#include "../Inc/c_pwm.hpp"
#include "pwm.h"
#include <cstring>
#include "mylibs_config.hpp"

void C_PWM::Init(u32 us, C_Pin* pins, u32 N_pin)
{
    u32 duty[N_pin];
    memset(duty, 0, N_pin);
    u32 (*pin_info_list)[3] = (u32(*)[3])XMalloc(sizeof(u32(*)[3])*N_pin);
    for(int i=0;i<N_pin;i++){
        *pin_info_list[0] = pins->IO_MUX();
        *pin_info_list[1] = pins->IO_FUNC();
        *pin_info_list[3] = pins->get_pin_no();
    }
    pwm_init(us, duty, N_pin, pin_info_list);
    XFree(pin_info_list);
}

void C_PWM::set_duty(u32 duty, u8 channel)
{
    pwm_set_duty(duty, channel);
}

void C_PWM::set_period(u32 us)
{
    pwm_set_period(us);
}