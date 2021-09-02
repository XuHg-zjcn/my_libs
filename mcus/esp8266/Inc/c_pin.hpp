#ifndef C_PIN_HPP
#define C_PIN_HPP

#include "i_pin.hpp"
#include "myints.h"

typedef enum{
    EXTI_Disable = 0,
    EXTI_Rise,
    EXTI_Fall,
    EXTI_Edge,
    EXTI_Low,
    EXTI_High
}EXTI_Type;

class C_Pin : public I_Pin{
public:
    u8 pin_no;
    C_Pin(u8 pin_no);
    void write_pin(PinState x);
    PinState read_pin();
    inline u8 get_pin_no() {return pin_no;}
    void loadCfg(PinCfg cfg);
    //从i_pin.cpp/hpp复制
    void toggle_pin();
    void wait_pin(PinState state);
    u32 wait_timeout(PinState state, u32 timeout);
    u32 wait_count(PinState state, u32 m, u32 M);
    //中断
    /*void set_EXTI_type(EXTI_Type type);
    void set_EXTI_isr(void (*fn)(void *), void *arg);
    void EXTI_enable();
    void EXTI_disable();*/
};

#endif
