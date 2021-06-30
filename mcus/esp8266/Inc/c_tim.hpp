#ifndef C_TIM_HPP
#define C_TIM_HPP

#include "common/Inc/myints.h"

class C_TIM{
public:
    void init(void);
    void arm(u32 val, bool req);
    void disarm(void);
    void set_us(u32 us);
    void set_callback(void (* user_hw_timer_cb_set)(void));
};

#endif