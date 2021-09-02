#include "esp8266/Inc/c_tim.hpp"
#include "driver/hw_timer.h"

void C_TIM::init(void (*callback)(void*), void *arg)
{
    hw_timer_init(callback, arg);
}

void C_TIM::set_load_data(u32 val)
{
    hw_timer_set_load_data(val);
}

void C_TIM::disarm()
{
    hw_timer_disarm();
}

void C_TIM::set_us(u32 us)
{
    hw_timer_alarm_us(us, true);
}

void C_TIM::set_callback(void (* user_hw_timer_cb_set)(void))
{
    //hw_timer_set_func(user_hw_timer_cb_set);
}
