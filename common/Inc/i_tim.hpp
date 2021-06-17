#ifndef I_TIM_HPP
#define I_TIM_HPP

#include "myints.h"

class I_TIM{
public:
    void Init();
    void set_us(u32 us);
    void set_callback(void (*func)(void));
    void start();
    void stop();
};

#endif