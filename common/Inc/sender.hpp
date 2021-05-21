/*
 * sender.hpp
 *
 *  Created on: 2021年5月11日
 *      Author: xrj
 */

#ifndef DATAFLOW_INC_SENDER_HPP_
#define DATAFLOW_INC_SENDER_HPP_

#include "buffer.hpp"
#include "cmsis_os2.h"
#include "usbd_customhid.h"
#include "ssd1306.hpp"


typedef struct{
	Buffer *buf;
	USBD_HandleTypeDef *pdev;
	u32 n;
}SenderParam;

typedef struct{
	Buffer *buf;
	SSD1306 *oled;
}ShowParam;

void UsbSender_ProcFunc(void *argument);

#endif /* DATAFLOW_INC_SENDER_HPP_ */
