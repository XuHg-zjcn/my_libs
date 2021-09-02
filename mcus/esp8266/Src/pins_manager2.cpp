/*
 * pin_manager2.cpp
 *
 *  Created on: Jun 12, 2021
 *      Author: xrj
 */

#include "pins_manager2.hpp"

uint16_t pins_use_state;  //TODO: use FreeRTOS event flags, can wait

bool isPinUsed(C_Pin &pin)
{
    if(pins_use_state & 1<<pin.pin_no){
        return true;
    }else{
        return false;
    }
}

void setPinUsed(C_Pin &pin, bool used)
{
    if(used){
        pins_use_state |= 1<<pin.pin_no;
    }else{
        pins_use_state &= ~(1<<pin.pin_no);
    }
}


