/*
 * CH32V103C8T6 USB从机模式驱动库
 * Copyright (C) Xu Ruijun  2022
 * 部分代码从WCH官方例程`CH32V103EVT.ZIP`压缩包中复制
 */

#include "c_usb.hpp"
#include "c_pin.hpp"
#include "usb_std.h"
#include "myints.h"
#include "ops.hpp"
#include <cstring>
#include "myints.h"

u8 C_USB_Regs::* const C_USB_MOD_REG[7] = {
  &C_USB_Regs::UEP4_1_MOD,  //1
  &C_USB_Regs::UEP2_3_MOD,  //2
  &C_USB_Regs::UEP2_3_MOD,  //3
  &C_USB_Regs::UEP4_1_MOD,  //4
  &C_USB_Regs::UEP5_6_MOD,  //5
  &C_USB_Regs::UEP5_6_MOD,  //6
  &C_USB_Regs::UEP7_MOD     //7
};

const u8 C_USB_MOD_SFT[7] = {
  4, 0, 4, 0, 0, 4, 0
};

/*
 * 不知道为什么写寄存器必须先取地址再取内容，否则无效。
 * 可行：`*(&reg) = x;`，不行：`reg = x;`
 * 目前定义了CAST宏来实现操作，但还是有点麻烦，不知道怎么样才能省去这些步骤？
 */

/* Global Variable */
#define DevEP0SIZE	0x40

C_USBD::C_USBD(volatile void *baddr):
  regs((volatile C_USB_Regs *)baddr){};

int C_USBD::Init(const void *DeviceDescr,
                 const void *ConfigDescr,
                 const void *StringDescr)
{
  extern uint32_t SystemCoreClock;

  this->DeviceDescr = (const u8 *)DeviceDescr;
  this->ConfigDescr = (const u8 *)ConfigDescr;
  this->StringDescr = (const u8 *)StringDescr;

  const u8 *pTmp = this->ConfigDescr;
  //Malloc返回指针应该是对齐的
  pEP0_RAM_Addr = (u8 *)XMalloc(64);

  //操作寄存器前必须初始化时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  EXTEN->EXTEN_CTR |= EXTEN_USBHD_IO_EN;
  switch(SystemCoreClock){  //PLL必须72MHz或48MHz
  case 72000000:
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
    break;
  case 48000000:
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div1);
    break;
  default:
    return 2;
    break;
  }
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBHD, ENABLE);
  CAST(u8, regs->USB_CTRL) = 0x00;  //复位

  CAST(u16, regs->UEPx_DMA[0].L16b) = (u16)(u32)pEP0_RAM_Addr;
  CAST(u8, regs->UEPx_TLEN_CTRL[0].CTRL) = UEP_R_RES_ACK | UEP_T_RES_NAK;

  //TODO: 添加端点映射支持
  //手册上宣称支持映射端点，但没有具体描述
  while(pTmp[0] >= 2){
    if(pTmp[1] == USB_DescrType_Endp){
      u8 endp = pTmp[2];
      if((endp&0x7F) >= 8){
        return 1;  //only support 7 ports
      }
      if(endp&0x80){
        CAST(u8, regs->UEPx_TLEN_CTRL[endp&0x7f].CTRL) |= UEP_T_RES_NAK;
		CAST(u8, regs->*C_USB_MOD_REG[(endp&0x7f)-1]) |= RB_UEP2_TX_EN << C_USB_MOD_SFT[(endp&0x7f)-1];
      }else{
        CAST(u8, regs->UEPx_TLEN_CTRL[endp&0x7f].CTRL) |= UEP_R_RES_ACK;
		CAST(u8, regs->*C_USB_MOD_REG[(endp&0x7f)-1]) |= RB_UEP2_RX_EN << C_USB_MOD_SFT[(endp&0x7f)-1];
      }
    }
    pTmp += pTmp[0];
  }

  DevInit();
  NVIC_EnableIRQ(USBHD_IRQn);
  return 0;
}

int C_USBD::Malloc_Buff(const u8 *Endps)
{
  const u8 *endp_i = Endps;
  int count = 0;
  while(*endp_i){
	int i = *endp_i;
    u8 tmp = regs->*C_USB_MOD_REG[i-1] >> C_USB_MOD_SFT[i-1];
	count += (tmp&0b1000 + tmp&0b0100)*(tmp&0b0001?2:1);
	endp_i++;
  }
  if(count == 0){
	return 0;
  }
  u8 *p = (u8*)XMalloc(count*64);
  endp_i = Endps;
  while(*endp_i){
	int i = *endp_i;
    u8 tmp = regs->*C_USB_MOD_REG[i-1] >> C_USB_MOD_SFT[i-1];
	regs->UEPx_DMA[i].L16b = (u16)(u32)p;
	p += (tmp&0b1000 + tmp&0b0100)*(tmp&0b0001?2:1)*64;
	endp_i++;
  }
  return count;
}

void C_USBD::DevInit()
{
  CAST(u8, regs->INT_FG) = 0xFF;
  CAST(u8, regs->INT_EN) = RB_UIE_SUSPEND | RB_UIE_BUS_RST | RB_UIE_TRANSFER;

  CAST(u8, regs->DEV_AD) = 0x00;
  CAST(u8, regs->USB_CTRL) = RB_UC_DEV_PU_EN | RB_UC_INT_BUSY | RB_UC_DMA_EN;
  CAST(u8, regs->UDEV_CTRL) = RB_UD_PD_DIS | RB_UD_PORT_EN;
}

u32 C_USBD::StdDesc(u16 wValue, u8 *buffer, u32 ReqLen)
{
  u32 len;
  u8 errflag = 0;

  switch(wValue>>8){
  case USB_DESCR_TYP_DEVICE:
    pDescr = DeviceDescr;
    len = DeviceDescr[0];
    break;
  case USB_DESCR_TYP_CONFIG:
    pDescr = ConfigDescr;
    len = ConfigDescr[2];
    break;
  case USB_DESCR_TYP_STRING:
    //TODO: add multi-languange support
    wValue &= 0xff;
    pDescr = StringDescr;
    while(wValue){
      if(pDescr[0] == 0){
        break;
      }
      pDescr += pDescr[0];
      wValue--;
    }
    len = pDescr[0];
    break;
  default:
    errflag = 0xff;
    break;
  }
  if(errflag != 0){
    return 0;
  }
  if(ReqLen > len) ReqLen = len;
  len = (ReqLen >= DevEP0SIZE) ? DevEP0SIZE : ReqLen;
  memcpy(buffer, pDescr, len);
  pDescr += len;
  return len;
}

u8 C_USBD::StdReq(USB_SetupReq *ssreq)
{
  u8 errflag = 0;
  switch(SetupReqCode){
  case USB_GET_DESCRIPTOR:
    StdDesc(ssreq->wValue, pEP0_RAM_Addr, SetupReqLen);
    break;
  case USB_SET_ADDRESS:
    SetupReqLen = (ssreq->wValue)&0xff;
    break;
  case USB_GET_CONFIGURATION:
    pEP0_DataBuf[0] = DevConfig;
    if(SetupReqLen > 1) SetupReqLen = 1;
    break;
  case USB_SET_CONFIGURATION:
    DevConfig = (ssreq->wValue)&0xff;
    break;
  case USB_CLEAR_FEATURE:
    if((ssreq->bmRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP){
      switch( (ssreq->wIndex)&0xff ){
      case 0x82:
        R8_UEP2_CTRL = (R8_UEP2_CTRL & ~( RB_UEP_T_TOG|MASK_UEP_T_RES )) | UEP_T_RES_NAK;
        break;
      case 0x02:
        R8_UEP2_CTRL = (R8_UEP2_CTRL & ~( RB_UEP_R_TOG|MASK_UEP_R_RES )) | UEP_R_RES_ACK;
        break;
      case 0x81:
        R8_UEP1_CTRL = (R8_UEP1_CTRL & ~( RB_UEP_T_TOG|MASK_UEP_T_RES )) | UEP_T_RES_NAK;
        break;
      case 0x01:
        R8_UEP1_CTRL = (R8_UEP1_CTRL & ~( RB_UEP_R_TOG|MASK_UEP_R_RES )) | UEP_R_RES_ACK;
        break;
      default:
        errflag = 0xFF;
        break;
      }
    }else errflag = 0xFF;
    break;
  case USB_GET_INTERFACE:
    pEP0_DataBuf[0] = 0x00;
    if(SetupReqLen > 1) SetupReqLen = 1;
    break;
  case USB_GET_STATUS:
    pEP0_DataBuf[0] = 0x00;
    pEP0_DataBuf[1] = 0x00;
    if(SetupReqLen > 2) SetupReqLen = 2;
    break;
  default:
    errflag = 0xff;
    break;
  }  // end of switch(SetupReqCode
  return errflag;
}

i32 C_USBD::USB_ISR()
{
  UINT8  len, chtype;
  UINT8  intflag, errflag = 0;

  intflag = R8_USB_INT_FG;
  u8 endp = R8_USB_INT_ST & MASK_UIS_ENDP;

  if( intflag & RB_UIF_TRANSFER ){
    switch ( R8_USB_INT_ST & MASK_UIS_TOKEN) {
    case UIS_TOKEN_SETUP:
      CAST(u8, regs->UEPx_TLEN_CTRL[0].CTRL) = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
      len = regs->RX_LEN;
      if ( len == sizeof( USB_SETUP_REQ ) ){
        USB_SetupReq *SetupReqPak = ((USB_SetupReq *)pEP0_RAM_Addr);
        SetupReqLen = SetupReqPak->wLength;
        SetupReqCode = SetupReqPak->bRequest;
        chtype = SetupReqPak->bmRequestType;

        len = 0;
        errflag = 0;
        if((SetupReqPak->bmRequestType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD){
          errflag = 0xFF;
        }else{
          StdReq(SetupReqPak);
        }  // end of else block
      }  // end of `if( len == sizeof( USB_SETUP_REQ )`
      else errflag = 0xff;

      if(errflag == 0xff){
        R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;
      }else{
        if(chtype & 0x80){
          len = (SetupReqLen > DevEP0SIZE) ? DevEP0SIZE : SetupReqLen;
          SetupReqLen -= len;
        }else len = 0;
        R8_UEP0_T_LEN = len;
        R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
      }
      break;

    case UIS_TOKEN_IN:
      if(endp == 0){
        switch(SetupReqCode){
        case USB_GET_DESCRIPTOR:
          len = SetupReqLen >= DevEP0SIZE ? DevEP0SIZE : SetupReqLen;
          SetupReqLen -= len;
          memcpy(pEP0_DataBuf, pDescr, len);
          pDescr += len;
          Send_Pack(0, len);
          break;
        case USB_SET_ADDRESS:
          R8_USB_DEV_AD = (R8_USB_DEV_AD&RB_UDA_GP_BIT) | SetupReqLen;
          R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
          break;
        default:
          R8_UEP0_T_LEN = 0;
          R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
          break;
        }
      }else{
        TOGG_BIT(CAST(u8, regs->UEPx_TLEN_CTRL[endp].CTRL), RB_UEP_T_TOG);
        MODIFY_REG(CAST(u8, regs->UEPx_TLEN_CTRL[endp].CTRL), MASK_UEP_T_RES, UEP_T_RES_NAK);
      }
      break;

    case UIS_TOKEN_OUT:
      len = R8_USB_RX_LEN;
      break;

    case UIS_TOKEN_SOF:
      break;

    default:
      break;
    }
    R8_USB_INT_FG = RB_UIF_TRANSFER;
    if(!(R8_USB_INT_ST & RB_UIS_TOKEN0)){
      return endp | ((R8_USB_INT_ST & RB_UIS_TOKEN1) << 2);
    }else{
      return -1;
    }
  }else if( intflag & RB_UIF_BUS_RST ){
    R8_USB_DEV_AD = 0;
    R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    R8_UEP1_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    R8_UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    R8_UEP3_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    R8_UEP4_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    R8_UEP5_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    R8_UEP6_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    R8_UEP7_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    R8_USB_INT_FG |= RB_UIF_BUS_RST;
  }else if( intflag & RB_UIF_SUSPEND ){
    R8_USB_INT_FG = RB_UIF_SUSPEND;
  }else{
    R8_USB_INT_FG = intflag;
  }
  return -1;
}

int C_USBD::Send_Pack(u8 endp, u16 len)
{
  if(endp&0x80 == 0){
    return 1;
  }
  endp &= 0x7f;
  if(endp >= 8){
    return 1;
  }else{
    regs->UEPx_TLEN_CTRL[endp].T_LEN = len;
    MODIFY_REG(regs->UEPx_TLEN_CTRL[endp].CTRL,
               MASK_UEP_T_RES,
               UEP_T_RES_ACK);
    return 0;
  }
}

int C_USBD::Send_Pack(u8 endp, void *p, u16 len)
{
  if(endp&0x80 == 0){
    return 1;
  }
  endp &= 0x7f;
  if(endp >= 8){
    return 1;
  }
  regs->*C_USB_MOD_REG[endp-1] &= ~(0x0f << C_USB_MOD_SFT[endp-1]);
  regs->UEPx_DMA[endp].L16b = ((u16)(u32)p)&0xfffc;
  regs->UEPx_TLEN_CTRL[endp].T_LEN = len;
  regs->*C_USB_MOD_REG[endp-1] |= RB_UEP2_TX_EN << C_USB_MOD_SFT[endp-1];
  MODIFY_REG(regs->UEPx_TLEN_CTRL[endp].CTRL,
             MASK_UEP_T_RES,
             UEP_T_RES_ACK);
}

u8 *C_USBD::Get_Buffer(u8 endp)
{
  u8 Tmp;
  u8 *pTmp;
  if((endp&0x7f) >= 8){
    return nullptr;
  }
  pTmp = (u8*)(0x20000000 + regs->UEPx_DMA[endp&0x7f].L16b);
  if((endp&0x7f) == 0){
    return pTmp;
  }
  Tmp = regs->*C_USB_MOD_REG[(endp&0x7f)-1];
  Tmp >>= C_USB_MOD_SFT[(endp&0x7f)-1];
  Tmp &= 0x0f;
  if(((Tmp&0b1100)==0b1000 && !(endp&0x80)) ||
     ((Tmp&0b1100)==0b0100 && (endp&0x80))){
    return pTmp;
  }
  if(Tmp==0b1100){
    if(endp&0x80){
      return pTmp + ((Tmp&0b0001)?128:64); //Tx(IN)
    }else{
      return pTmp;    //Rx(OUT)
    }
  }
  return nullptr;
}
