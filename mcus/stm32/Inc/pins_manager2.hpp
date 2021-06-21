/*
 * pin_manager2.hpp
 *
 *  Created on: Jun 12, 2021
 *      Author: xrj
 */

#ifndef STM32_INC_PINS_MANAGER2_HPP_
#define STM32_INC_PINS_MANAGER2_HPP_


#include "mylibs_config.hpp"
#include "c_pin.hpp"

bool isPinUsed(C_Pin &pin);
void setPinUsed(C_Pin &pin, bool used);


#endif /* STM32_INC_PINS_MANAGER2_HPP_ */
