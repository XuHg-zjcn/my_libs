/*
 * c_tim.hpp for CH32V103C8T6
 *
 *  Created on: Jan 9, 2022
 *      Author: xrj
 */

#include "ch32v10x_tim.h"
#include "c_pin.hpp"
#if !defined(__C_TIM_HPP__) && \
     defined(__CH32V10x_TIM_H) && \
     defined(__C_PIN_HPP__)
#define __C_TIM_HPP__

#include "mylibs_config.hpp"
#include "myints.h"
#include "ops.hpp"

#define N_IT     8           //number interrupts of timer
#define CompKeep 0xffffffff  //flag for `set_comp4()` keep old compare value.
#define DutyKeep -1          //flag for `set_duty4()` keep old compare value.

//TODO: use #include<ratio>
typedef uint64_t Type_ns;
typedef uint32_t Type_Hz;
typedef uint64_t TypeDiv;

//enums for timer operate
typedef enum{
  TIM_ClkLv_APBx = 0,
  TIM_ClkLv_Mult2,
  TIM_ClkLv_CKD,
  TIM_ClkLv_PSC16b,
  TIM_ClkLv_AutoLoad
}TIM_ClockLevel;

//enums for channel operate
typedef enum{
  CTIM_Channel_1 = 0,
  CTIM_CHannel_1N,
  CTIM_Channel_2,
  CTIM_Channel_2N,
  CTIM_Channel_3,
  CTIM_Channel_3N,
  CTIM_Channel_4,
  CTIM_Channel_4N,
}CTIM_CHx;

typedef struct{
  bool TIM_Update:1;
  bool TIM_CC1:1;
  bool TIM_CC2:1;
  bool TIM_CC3:1;
  bool TIM_CC4:1;
  bool TIM_COM:1;
  bool TIM_Trigger:1;
  bool TIM_Break:1;
}CTIM_Event;

typedef struct{
  CTIM_Event ITE;
  CTIM_Event DMA;
}CTIM_ITDMA;

typedef struct{
  CTIM_Event ITF;
  CTIM_Event Rep;
}CTIM_INTFR;

typedef enum{
  CTIM_TRGO_Reset  = 0b000,
  CTIM_TRGO_Enable = 0b001,
  CTIM_TRGO_Update = 0b010,
  CTIM_TRGO_OC1    = 0b011,
  CTIM_TRGO_OC1Ref = 0b100,
  CTIM_TRGO_OC2Ref = 0b101,
  CTIM_TRGO_OC3Ref = 0b110,
  CTIM_TRGO_OC4Ref = 0b111,
}CTIM_TRGO;

//CCMR_comm
typedef enum{
  CTIM_Out   = 0b00,  //输出模式
  CTIM_In    = 0b01,  //输入模式
  CTIM_InX   = 0b10,  //输入模式，交换通道
  CTIM_InTRC = 0b11,  //输入模式，Slave模式
}CTIM_CCxS;

//CCMR_Out
typedef enum{
  CTIM_OCM_Timing         = 0b000,
  CTIM_OCM_Match_Active   = 0b001,
  CTIM_OCM_Match_Inactive = 0b010,
  CTIM_OCM_Match_Toggle   = 0b011,
  CTIM_OCM_Force_Inactive = 0b100,
  CTIM_OCM_Force_Active   = 0b101,
  CTIM_OCM_PWM_Mode1      = 0b110,
  CTIM_OCM_PWM_Mode2      = 0b111,
}CTIM_OCxM;

typedef enum{
  CMS_Edge = 0,
  CMS_Cen1 = 1,
  CMS_Cen2 = 2,
  CMS_Cen3 = 3
}CTIM_CMS;

typedef enum{
  CTIM_CKD_No = 0,
  CTIM_CKD_2  = 1,
  CTIM_CKD_4  = 2,
}CTIM_CKD;

typedef enum{
  CTIM_SMS_CKINT = 0b000,
  CTIM_SMS_ENCM1 = 0b001,
  CTIM_SMS_ENCM2 = 0b010,
  CTIM_SMS_ENCM3 = 0b011,
  CTIM_SMS_RESET = 0b100,
  CTIM_SMS_GATE  = 0b101,
  CTIM_SMS_TRIG  = 0b110,
  CTIM_SMS_EXTC  = 0b111
}CTIM_SMS;

typedef enum{
  CTIM_TS_ITR0    = 0b000,
  CTIM_TS_ITR1    = 0b001,
  CTIM_TS_ITR2    = 0b010,
  CTIM_TS_ITR3    = 0b011,
  CTIM_TS_TI1F_ED = 0b100,
  CTIM_TS_TI1FP1  = 0b101,
  CTIM_TS_TI2FP2  = 0b110,
  CTIM_TS_ETRF    = 0b111
}CTIM_TS;

typedef enum{
  CTIM_Filter_No      = 0b0000,
  CTIM_Filter_CKINT_2 = 0b0001,
  CTIM_Filter_CKINT_4 = 0b0010,
  CTIM_Filter_CKINT_8 = 0b0011,
  CTIM_Filter_DTS2_6  = 0b0100,
  CTIM_Filter_DTS2_8  = 0b0101,
  CTIM_Filter_DTS4_6  = 0b0110,
  CTIM_Filter_DTS4_8  = 0b0111,
  CTIM_Filter_DTS8_6  = 0b1000,
  CTIM_Filter_DTS8_8  = 0b1001,
  CTIM_Filter_DTS16_5 = 0b1010,
  CTIM_Filter_DTS16_6 = 0b1011,
  CTIM_Filter_DTS16_8 = 0b1100,
  CTIM_Filter_DTS32_5 = 0b1101,
  CTIM_Filter_DTS32_6 = 0b1110,
  CTIM_Filter_DTS32_8 = 0b1111,
}CTIM_Filter;

typedef enum{
  CTIM_PSC_NoDiv = 0b00,
  CTIM_PSC_Div2 = 0b01,
  CTIM_PSC_Div4 = 0b10,
  CTIM_PSC_Div8 = 0b11,
}CTIM_PSC;

#pragma pack(1)
typedef struct{
  bool CEN:1;
  bool UDIS:1;
  bool URS:1;
  bool OPM:1;
  bool DIR:1;
  CTIM_CMS CMS:2;
  bool ARPE:1;
  CTIM_CKD CKD:2;
  unsigned int resv:6;
}CTIM_CTLR1;

typedef struct{
  bool CCPC:1;
  bool resv1:1;
  bool CCUS:1;
  bool CCDS:1;
  CTIM_TRGO MMS:3;
  bool TI1S:1;
  bool OIS1:1;
  bool OSI1N:1;
  bool OIS2:1;
  bool OSI2N:1;
  bool OIS3:1;
  bool OSI3N:1;
  bool OIS4:1;
  bool resv2:1;
}CTIM_CTLR2;

typedef struct{
  CTIM_SMS SMS:3;
  bool resv1:1;
  CTIM_TS TS:3;
  bool MSM:1;
  CTIM_Filter ETF:4;
  CTIM_PSC ETPS:2;
  bool ECE:1;
  bool ETP:1;
}CTIM_SMCFGR;

typedef struct{
  CTIM_CCxS       ccxs:2;
  FunctionalState fasten:1;
  FunctionalState preload:1;
  CTIM_OCxM       ocxm:3;
  FunctionalState clear:1;
}CTIM_CCMR_Out;
//CCMR_Out end

typedef struct{
  CTIM_CCxS  ccxs:2;
  CTIM_PSC   icpsc:2;
  CTIM_Filter icf:4;
}CTIM_CCMR_In;

typedef union{
  CTIM_CCMR_Out Out;
  CTIM_CCMR_In  In;
}CTIM_CCMR;

typedef struct{
  CTIM_CCMR L;
  CTIM_CCMR H;
}CTIM_CHCTLR;
#pragma pack()

#pragma pack(2)
typedef struct{
  __IO CTIM_CTLR1 CTLR1;     u16 resv1;
  __IO CTIM_CTLR2 CTLR2;     u16 resv2;
  __IO CTIM_SMCFGR SMCFGR;   u16 resv3;
  __IO CTIM_ITDMA DMAINTENR; u16 resv4;
  __IO CTIM_INTFR INTFR;     u16 resv5;
  __IO CTIM_Event SWEVGR;    u16 resv6;
  __IO CTIM_CHCTLR CHCTLR1;  u16 resv7;
  __IO CTIM_CHCTLR CHCTLR2;  u16 resv8;
  __IO u16 CCER;             u16 resv9;
  __IO u16 CNT;              u16 resv10;
  __IO u16 PSC;              u16 resv11;
  __IO u16 ATRLR;            u16 rscv12;
  __IO u16 RPTCR;            u16 resv13;
  struct{
    __IO u16 CMP;
    u16 resv;
  }CHxCVR[4];
  __IO u16 BDTR;             u16 resv14;
  __IO u16 DMACFGR;          u16 resv15;
  __IO u16 DMAADR;           u16 resv16;
}CTIM_Regs;
#pragma pack()

class C_TIM : public CTIM_Regs{
public:
  inline void Start() {CTLR1.CEN = true;};
  inline void Stop()  {CTLR1.CEN = false;};
  //Trig and Clock
  void set_Trig(u32 trig, u32 polar, u32 prescale, u32 filter);
  void set_ExtClk(bool isExtern);
  //high level API get/set ClockDiv(CKD), PreScale(PSC), AutoLoad(ARR)
  TypeDiv get_div(TIM_ClockLevel level);
  Type_Hz get_Hz(TIM_ClockLevel level);
  Type_ns get_ns(TIM_ClockLevel level);
  void set_div(TypeDiv div);
  void set_Hz(Type_Hz Hz);
  void set_ns(Type_ns ns);
  //others
  void set_TRGO(CTIM_TRGO trgo, bool ms_enable);
  void set_CountEnable(bool isEnable);
  //inline void set_CountMode(TIM_CountMode mode) {MODIFY_REG(this->CTRL1, TIM_CR1_DIR|TIM_CR1_CMS, mode);};
  /*inline void reset_count(){
    uint32_t cnt = __HAL_TIM_IS_TIM_COUNTING_DOWN(htim)?__HAL_TIM_GET_AUTORELOAD(htim):0;
    __HAL_TIM_SET_COUNTER(htim, cnt);
    }*/
  u32 maxcount();

  //TODO: set comp ns
  //TODO: add DMA support
  //operate single channel
  u16  get_comp(CTIM_CHx Channel);
  u16  get_duty(CTIM_CHx Channel);
  void set_comp(CTIM_CHx Channel, u16 comp);
  void set_duty(CTIM_CHx Channel, u16 duty);
  //operate 4 channels
  void set_comp4(u32 comp1, u32 comp2, u32 comp3, u32 comp4);
  void set_duty4(float duty1, float duty2, float duty3, float duty4);
  //OnePluse Mode
  void pluse_clk(CTIM_CHx Channel, u32 delay_clk, u32 pluse_clk, bool blocking);
  void pluse_ns(CTIM_CHx Channel, u32 delay_ns, u32 pluse_ns, bool blocking);
  //others
  void CCxChannelCmd(CTIM_CHx Channel, FunctionalState ChannelState);
  void CCxChannelPolar(CTIM_CHx Channel, PinState ActiveLevel);

  X_State Set_CCMR(CTIM_CHx Channel, CTIM_CCMR ccmr);

  void PWM_Init();
  void PWM_Start(CTIM_CHx Channel);
  void Force_Active(CTIM_CHx Channel);
  void Force_Inactive(CTIM_CHx Channel);

  inline uint32_t ReadCapturedValue(CTIM_CHx Channel);
  //old C_TIMEx
  //void set_callback(CTIM_IT IT, void (*func)(void*), void* param);
  //void clear_callback(CTIM_IT IT);
  //void from_ISR();
};



//single channel class
class TIM_CH{
private:
  C_TIM *htim;
  CTIM_CHx Channel;
  bool allowCNT;
public:
  TIM_CH(C_TIM *htim, CTIM_CHx Channel, bool allowCNT);
  inline u16  get_comp()    {return htim->get_comp(Channel);}
  inline u16  get_duty()    {return htim->get_duty(Channel);}
  inline void set_comp(u16 comp)   {htim->set_comp(Channel, comp);}
  inline void set_duty(u16 duty) {htim->set_duty(Channel, duty);}
  //OnePluse Mode
  void pluse_ns(u32 delay_ns, u32 pluse_ns, bool blocking);
  void pluse_clk(u32 delay_clk, u32 pluse_clk, bool blocking);
  //others
  //void set_OCMode(TIM_OCMode mode);
  //void CCxChannelCmd(TIM_CCxE ChannelState);

  Type_Hz get_Hz(TIM_ClockLevel level){return htim->get_Hz(level);}
  Type_ns get_ns(TIM_ClockLevel level){return htim->get_ns(level);}
  void set_Hz(Type_Hz Hz)             {htim->set_Hz(Hz);}
  void set_ns(Type_ns ns)             {htim->set_ns(ns);}

  //void EnableIT()           {htim->EnableIT(TIM_CH2IT(Channel));}
  //void DisableIT()          {htim->DisableIT(TIM_CH2IT(Channel));}

  //void start()	{CCxChannelCmd(ENABLE);}
  //void stop()	{CCxChannelCmd(DISABLE);}
};

extern "C"{
  void C_TIMEx_ISR_func(C_TIM* ctim);
}

#endif /* INC_TIMER_HPP_ */
