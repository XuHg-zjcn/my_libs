/*
 * pin_manager2.hpp
 *
 *  Created on: Jun 12, 2021
 *      Author: xrj
 */

#ifndef PINS_MANAGER2_HPP_
#define PINS_MANAGER2_HPP_


#include "mylibs_config.hpp"
#include "c_pin.hpp"

bool isPinUsed(C_Pin &pin);
void setPinUsed(C_Pin &pin, bool used);


#endif /* PINS_MANAGER2_HPP_ */
