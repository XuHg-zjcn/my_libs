/*
 * c_exti.hpp
 *
 *  Created on: 2021年5月19日
 *      Author: xrj
 */

#ifndef STM32_INC_F1_EXTI_HPP_
#define STM32_INC_F1_EXTI_HPP_

#include "f1_gpio.hpp"

typedef enum{
    EXTI_DISABLE0 = 0,
    EXTI_IT_RISE,
    EXTI_IT_FALL,
    EXTI_IT_RISE_FALL,
    EXTI_DISABLE1,
    EXTI_EVT_RISE,
    EXTI_EVT_FALL,
    EXTI_EVT_RISE_FALL
}EnumEXTI;
#define EXTI_RISE_Msk 0b01
#define EXTI_FALL_Msk 0b10
#define EXTI_EVT_Msk 0b100

void setPinEXTI(Pin_8b pin, EnumEXTI exti);


#endif /* STM32_INC_C_EXTI_HPP_ */
