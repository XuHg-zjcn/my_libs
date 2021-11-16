/*
 * c_uart.hpp
 *
 *  Created on: Nov 16, 2021
 *      Author: xrj
 */

#include "mylibs_config.hpp"
#if !defined(STM32_INC_C_UART_HPP_) && defined(HAL_UART_MODULE_ENABLED)
#define STM32_INC_C_UART_HPP_

#include "x_base.hpp"

class C_UART : public UART_HandleTypeDef{
public:
	X_State send(u8* pData, u32 Size);
	X_State recv(u8* pData, u32 Size);
	i16 recv_byte(u32 timeout);
};

//地址识别，还没实现
typedef struct{
	u32 ticks; //头前至少空闲ms
	u8 *head;
	u32 head_len;
	void (*func)();
}C_UART_Dev;

class C_UART_Ex{
private:
	u8 head[8];  //目前头
	u32 len;     //接收长度
	u32 last_ts; //最后一次接收数据
	//设备
	u32 min_tick;  //设备中ticks最小值，两包间隔小于此数不用扫描设备
	C_UART_Dev *devs;
	u32 N_dev;
public:
	void ISR_func(u8 byte);
};

#endif /* STM32_INC_C_UART_HPP_ */
