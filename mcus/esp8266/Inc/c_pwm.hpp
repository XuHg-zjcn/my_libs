#ifndef C_PWM_HPP
#define C_PWM_HPP

#include "common/Inc/i_pwm.hpp"
#include "common/Inc/myints.h"
#include "c_pin.hpp"

class C_PWM{
public:
    void Init(u32 us, C_Pin* pins, u32 N_pin);
    void set_duty(u32 duty, u8 channel);
    u32  get_duty(u8 channel);
    void set_period(u32 us);
    u32  get_period(void);
    void start(void);
};

class C_PWM_CH : public I_PWM_CH{
private:
    C_PWM* cpwm;
    u8 channel;
public:
    void set_duty(float duty)   {cpwm->set_duty(65536*duty, channel);}
    void set_Hz(u32 Hz)         {cpwm->set_period(1000000U/Hz);}
    void start()                {cpwm->start();}
    void stop()                 {cpwm->set_duty(0, channel);cpwm->start();}
};

#endif