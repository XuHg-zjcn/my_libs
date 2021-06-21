#ifndef I_PWM_HPP
#define I_PWM_HPP

class I_PWM_CH{
public:
    void set_duty(float duty);  //TODO: 16bit duty
    void set_Hz(u32 Hz);
    void start();
    void stop();
};

#endif