#include "ch32v10x.h"
#include "ch32v10x_usb.h"
#include "usb_std.h"
#include "myints.h"

#if !defined(__C_USB_HPP__) && \
     defined(__CH32V10x_USB_H)
#define __C_USB_HPP__


typedef struct{
  u8 USB_CTRL;   //+00 USB 控制寄存器
  u8 UDEV_CTRL;  //+01 USB 设备物理端口控制寄存器
  u8 INT_EN;     //+02 USB 中断使能寄存器
  u8 DEV_AD;     //+03 USB 中断使能寄存器
  u8 STATUS;     //+04 USB 状态寄存器(32b)
  u8 MIS_ST;     //+05 USB 杂项状态寄存器
  u8 INT_FG;     //+06 USB 中断标志寄存器
  u8 INT_ST;     //+07 USB 中断状态寄存器
  u8 RX_LEN;     //+08 USB 接收长度寄存器
  u8 resv0;      //+09
  u8 resv1;      //+0a
  u8 resc2;      //+0b
  u8 UEP4_1_MOD; //+0c 端点 1(9)/4(8/12)模式控制寄存器
  u8 UEP2_3_MOD; //+0d 端点 2(10)/3(11)模式控制寄存器
  u8 UEP5_6_MOD; //+0e 端点 5(13)/6(14)模式控制寄存器
  u8 UEP7_MOD;   //+0f 端点 7(15)模式控制寄存器
  struct{
    u16 L16b;
    u16 resv;
  }UEPx_DMA[8];
  struct{
    u16 T_LEN;
    u8 CTRL;
    u8 resv;
  }UEPx_TLEN_CTRL[8]; //+(30-4e)
}C_USB_Regs;


class C_USBD{
private:
  const u8 *DeviceDescr;
  const u8 *ConfigDescr;
  const u8 *StringDescr;
  u8 DevConfig;
  u8 SetupReqCode;
  u16 SetupReqLen;
  const u8 *pDescr;
  void (*Out_Callback)(u8 endp, u8* buff, u32 len);
  u32 StdDesc(u16 wValue, u8 *buffer, u32 reqlen);
  u8 StdReq(USB_SetupReq *ssreq);
  void DevInit();
public:
  volatile C_USB_Regs *const regs;
  C_USBD(volatile void *baddr);
  int Init(const void *DeviceDescr,
           const void *ConfigDescr,
           const void *StringDescr);
  int Malloc_Buff(const u8 *Endps);
  i32 USB_ISR();
  int Send_Pack(u8 endp, u16 len);
  int Send_Pack(u8 endp, void *p, u16 len);
  u8 *Get_Buffer(u8 endp);
};

#endif
