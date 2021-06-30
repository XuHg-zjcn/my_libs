#include "esp8266/Inc/c_tim.hpp"
#include "hw_timer.h"

void C_TIM::init()
{
    hw_timer_init();
}

void C_TIM::arm(u32 val, bool req)
{
    hw_timer_arm(val, req);
}

void C_TIM::disarm()
{
    hw_timer_disarm();
}

void C_TIM::set_us(u32 us)
{
    arm(us, true);
}

void C_TIM::set_callback(void (* user_hw_timer_cb_set)(void))
{
    hw_timer_set_func(user_hw_timer_cb_set);
}