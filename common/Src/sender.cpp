/*
 * usb_sender.cpp
 *
 *  Created on: 2021年5月11日
 *      Author: xrj
 */

#include "sender.hpp"
#include "buffhead.hpp"
#include "main.h"

void UsbSender_ProcFunc(void *argument)
{
	SenderParam *param = (SenderParam*)argument;
	BuffHeadReads *r_heads = &param->buf->r_heads;
	BuffHeadRead head = BuffHeadRead(*r_heads, r_heads->new_head());
	u8* p;
	u32 be = head.bytes_elem();
	while(1){
		p = (u8*)head.get_frames(param->n);
		HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, GPIO_PIN_RESET);
		USBD_CUSTOM_HID_SendReport(param->pdev, p, param->n*be);
		HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, GPIO_PIN_SET);
	}
}
