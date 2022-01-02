/*
 * co2_c8s.hpp
 *
 *  Created on: Nov 16, 2021
 *      Author: xrj
 */

#include "c_uart.hpp"
#if !defined(__CO2_C8S_HPP__) && \
	defined(__C_UART_HPP__)
#define __CO2_C8S_HPP__

class CO2_C8S{
private:
	C_UART *uart;
	u8 data[14];
public:
	CO2_C8S(C_UART *uart);
	u16 wait_report();
	u16 force_read();
};

#endif /* __CO2_C8S_HPP__ */
