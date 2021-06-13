/*
 * c_gpio.hpp
 *
 *  Created on: 2021年6月13日
 *      Author: xrj
 */

#ifndef COMMON_INC_I_PIN_HPP_
#define COMMON_INC_I_PIN_HPP_

#include "myints.h"

typedef enum{
    Pin_Reset = 0,
    Pin_Set
}PinState;

inline PinState operator!(PinState s){
	return (PinState)(!s);
}

class I_Pin{
public:
//需实现
    void write_pin(bool x);
    PinState read_pin();
//已实现，可覆盖
    void toggle_pin();
    void wait_pin(PinState state);
    u32 wait_timeout(PinState state, u32 timeout);
    u32 wait_count(PinState state, u32 m, u32 M);
};

#endif /* COMMON_INC_I_PIN_HPP_ */
