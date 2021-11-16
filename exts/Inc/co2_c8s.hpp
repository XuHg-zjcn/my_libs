/*
 * co2_c8s.hpp
 *
 *  Created on: Nov 16, 2021
 *      Author: xrj
 */

#ifndef EXTS_INC_CO2_C8S_HPP_
#define EXTS_INC_CO2_C8S_HPP_

#include "c_uart.hpp"

class CO2_C8S{
private:
	C_UART *uart;
	u8 data[14];
public:
	CO2_C8S(C_UART *uart);
	u16 wait_report();
	u16 force_read();
};


#endif /* EXTS_INC_CO2_C8S_HPP_ */
