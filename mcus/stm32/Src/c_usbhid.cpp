/*
 * c_usbhid.cpp
 *
 *  Created on: Jun 5, 2021
 *      Author: xrj
 */

#include "c_usbhid.hpp"

u8 USBHID::send(u8* p, u16 len)
{
	return USBD_CUSTOM_HID_SendReport(this, p, len);
}

void USBHID::wait()
{
	USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef *)this->pClassData;
	if(this->dev_state == USBD_STATE_CONFIGURED){
		while(hhid->state != CUSTOM_HID_IDLE);
	}
}

//call USBD_GetRxCount get length
void USBHID::recv_callback(int8_t (*OutEvent)(u8 event_idx, u8 state))
{
	((USBD_CUSTOM_HID_ItfTypeDef *)this->pUserData)->OutEvent = OutEvent;
}

void USBHID::RegisterInterface(USBD_CUSTOM_HID_ItfTypeDef *fops)
{
	USBD_CUSTOM_HID_RegisterInterface(this, fops);
}
