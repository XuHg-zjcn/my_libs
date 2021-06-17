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
    Pin_PP0 = 0,
    Pin_PP1,
    Pin_OD0,
    Pin_OD1,
    Pin_InUp,
    Pin_InFlt,
    Pin_InDown
}PinCfg;

#define PIN_IS_OUT(x) (x<=Pin_OD1)
#define PIN_IS_IN(x)  (x>=Pin_InUp)

#define NEAREST_PINSPEEDD(x) Pin_SpdDefa

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
    void write_pin(PinState x);
    PinState read_pin();
    void loadCfg(PinCfg cfg, u8 MHz);
    void loadCfg(PinCfg cfg);
//已实现，可覆盖
    void toggle_pin();
    void wait_pin(PinState state);
    u32 wait_timeout(PinState state, u32 timeout);
    u32 wait_count(PinState state, u32 m, u32 M);
};

#endif /* COMMON_INC_I_PIN_HPP_ */
