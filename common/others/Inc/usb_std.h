/*
 * ref: https://www.usb.org/document-library/usb-20-specification
 * usb_20_20211008.zip/usb_20.pdf
 */

#ifndef __USB_STD_H__
#define __USB_STD_H__

#include "myints.h"

#define USB_UTF16_DESC(s) \
  struct{ \
    u8 bLength = sizeof(s)+2; \
    u8 bDescriptorType = 3; \
    char16_t str[sizeof(s)/2] = s;}

#pragma pack(1)

typedef struct{
  u8  bmRequestType;
  u8  bRequest;
  u16 wValue;
  u16 wIndex;
  u16 wLength;
}USB_SetupReq;

//Standard USB Descriptors
typedef enum{
  USB_DescrType_Device = 1,
  USB_DescrType_Config = 2,
  USB_DescrType_String = 3,
  USB_DescrType_Intf = 4,
  USB_DescrType_Endp = 5,
  USB_DescrType_Quaf = 6,
  USB_DescrType_OSpd = 7,
}USB_DescrType;

typedef enum{
  USB_V1_1 = 0x0110,
  USB_V2_0 = 0x0200,
}USB_bcdVer;

typedef struct{
  u8  bLength; // 18
  u8  bDescriptorType; // 1
  u16 bcdUSB;
  u8  bDeviceClass;
  u8  bDeviceSubClass;
  u8  bDeviceProtocol;
  u8  bMaxPacketSize0;
  u16 idVendor;
  u16 idProduct;
  u16 bcdDevice;
  u8  iManufacturer;
  u8  iProduct;
  u8  iSerialNumber;
  u8  bNumConfigurations;
}USB_DeviceDescr;

typedef struct{
  u8  bLength; // 9
  u8  bDescriptorType; // 2
  u16 wTotalLength;
  u8  bNumInterfaces;
  u8  bConfigurationValue;
  u8  iConfiguration;
  u8  bmAttributes;
  u8  bMaxPower;
}USB_ConfigDescr;

typedef struct{
  u8  bLength; // 9
  u8  bDescriptorType; // 4
  u8  bInterfaceNumber;
  u8  bAlternateSetting;
  u8  bNumEndpoints;
  u8  bInterfaceClass;
  u8  bInterfaceSubClass;
  u8  bInterfaceProtocol;
  u8  iInterface;
}USB_IntfDescr;

typedef struct{
  u8  bLength; // 7
  u8  bDescriptorType; // 5
  u8  bEndpointAddress;
  u8  bmAttributes;
  u16 wMaxPacketSize;
  u8  bInterval;
}USB_EndpDescr;

#pragma pack()

#endif
