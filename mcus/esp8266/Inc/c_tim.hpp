#ifndef C_TIM_HPP
#define C_TIM_HPP

#include "myints.h"

class C_TIM{
public:
    void init(void (*callback)(void*), void *arg);
    void set_load_data(u32 val);
    void disarm(void);
    void set_us(u32 us);
    void set_callback(void (* user_hw_timer_cb_set)(void));
};

#endif
