#include "app.h"
#include "delay.hpp"
#include "c_pin.hpp"
#include "c_usb.hpp"
#include "ops.hpp"

#pragma pack(1)

/* Device Descriptor */
const USB_DeviceDescr MyDeviceDescr = {
  0x12, 0x01,              // bLength, bDescriptorType
  0x0110,                  // bcdUSB v1.1
  0xFF, 0x80, 0x55,        // Class, SubClass, Protocol
  0x40,                    // bMaxPacketSize0
  0xFFFF, 0xA103, 0x0100,  // VID, PID, bcdDevice
  0x01, 0x02, 0x00,        // iManu, iProd, iSerNum
  0x01,                    // bNumConfigurations
};

/* Configration Descriptor */
const struct MyCfgDescr{
  USB_ConfigDescr c = {0x09, 0x02, sizeof(MyCfgDescr), 0x01, 0x01, 0x00, 0x80, 0x32};
  USB_IntfDescr  i  = {0x09, 0x04, 0x00, 0x00, 0x0E, 0xFF, 0x80, 0x55, 0x00};
  USB_EndpDescr e87 = {0x07, 0x05, 0x87, 0x02, 0x0040, 0x00};
  USB_EndpDescr e07 = {0x07, 0x05, 0x07, 0x02, 0x0040, 0x00};
  USB_EndpDescr e86 = {0x07, 0x05, 0x86, 0x02, 0x0040, 0x00};
  USB_EndpDescr e06 = {0x07, 0x05, 0x06, 0x02, 0x0040, 0x00};
  USB_EndpDescr e85 = {0x07, 0x05, 0x85, 0x02, 0x0040, 0x00};
  USB_EndpDescr e05 = {0x07, 0x05, 0x05, 0x02, 0x0040, 0x00};
  USB_EndpDescr e84 = {0x07, 0x05, 0x84, 0x02, 0x0040, 0x00};
  USB_EndpDescr e04 = {0x07, 0x05, 0x04, 0x02, 0x0040, 0x00};
  USB_EndpDescr e83 = {0x07, 0x05, 0x83, 0x02, 0x0040, 0x00};
  USB_EndpDescr e03 = {0x07, 0x05, 0x03, 0x02, 0x0040, 0x00};
  USB_EndpDescr e82 = {0x07, 0x05, 0x82, 0x02, 0x0040, 0x00};
  USB_EndpDescr e02 = {0x07, 0x05, 0x02, 0x02, 0x0040, 0x00};
  USB_EndpDescr e81 = {0x07, 0x05, 0x81, 0x02, 0x0040, 0x00};
  USB_EndpDescr e01 = {0x07, 0x05, 0x01, 0x02, 0x0040, 0x00};
  u8 tail = 0;
}MyConfigDescr;

/* String Descriptors */
const struct StringDescrs{
  struct MyLangDescr{
    u8  bLength = sizeof(MyLangDescr);
    u8  bDescriptorType = 0x03;
    u16 wLANGIDs[1] = {0x0409};
  }MyLangDescr;
  USB_UTF16_DESC(u"Xu Ruijun DIY") MyManuInfo;
  USB_UTF16_DESC(u"CH32V103C8T6") MyProdInfo;
  u8 tail = 0;
}MyStringDescrs;

#pragma pack()

C_USBD *usbd;

int app(void)
{
  usbd = new C_USBD(&R8_USB_CTRL);
  usbd->Init(&MyDeviceDescr,
	     &MyConfigDescr,
	     &MyStringDescrs);

  while(1){
    XDelayMs(50);
  }
  return 0;
}


void USBHD_Callback(void)
{
  usbd->USB_ISR();
}
