/*
 * c_exti.cpp
 *
 *  Created on: 2021年5月19日
 *      Author: xrj
 */

#include "f1_exti.hpp"


void setPinEXTI(Pin_8b pin, EnumEXTI exti){
    uint32_t pin2N = p8b_Pin2N(pin);
    if(exti & (EXTI_RISE_Msk | EXTI_FALL_Msk)){
        //config AFIO->EXTICR
        uint32_t temp = AFIO->EXTICR[pin.PINx >> 2u];
        CLEAR_BIT(temp, (0x0Fu) << (4u * (pin.PINx & 0x03u)));
        SET_BIT(temp, pin.PORTx << (4u * (pin.PINx & 0x03u)));
        AFIO->EXTICR[pin.PINx >> 2] = temp;
        //config rising/falling edge
        if(exti & EXTI_RISE_Msk){
            SET_BIT(EXTI->RTSR, pin2N);
        }if(exti & EXTI_FALL_Msk){
            SET_BIT(EXTI->FTSR, pin2N);
        }
        //config Interrupt/Event
        if(exti & EXTI_EVT_Msk){
            CLEAR_BIT(EXTI->IMR, pin2N);
            SET_BIT(EXTI->EMR, pin2N);
        }else{
            CLEAR_BIT(EXTI->EMR, pin2N);
            SET_BIT(EXTI->IMR, pin2N);
        }
    }
}
