/*
 * c_usbhid.hpp
 *
 *  Created on: Jun 5, 2021
 *      Author: xrj
 */
#include "usbd_custom_hid_if.h"
#include "myints.h"

class USBHID : public USBD_HandleTypeDef{
public:
	u8 send(u8* p, u16 len);
	void wait();
	void recv_callback(int8_t (* OutEvent)(u8 event_idx, u8 state));
	void RegisterInterface(USBD_CUSTOM_HID_ItfTypeDef *fops);
};
