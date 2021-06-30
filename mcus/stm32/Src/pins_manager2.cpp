/*
 * pin_manager2.cpp
 *
 *  Created on: Jun 12, 2021
 *      Author: xrj
 */

#include "pins_manager2.hpp"

uint16_t pins_use_state[TOTAL_GPIO_PORTS];  //TODO: use FreeRTOS event flags, can wait

bool isPinUsed(C_Pin &pin)
{
    if(pins_use_state[pin.PORTx] & 1<<pin.PINx){
        return true;
    }else{
        return false;
    }
}

void setPinUsed(C_Pin &pin, bool used)
{
    if(used){
        pins_use_state[pin.PORTx] |= 1<<pin.PINx;
    }else{
        pins_use_state[pin.PORTx] &= ~(1<<pin.PINx);
    }
}


