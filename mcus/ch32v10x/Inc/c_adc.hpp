#ifndef __C_ADC_HPP__
#define __C_ADC_HPP__

#include "ch32v10x.h"
#include "myints.h"
#include "ops.hpp"
#include "c_tim.hpp"
#include "c_dma.hpp"

#define CADC_CHx_Msk  0b11111
#define CADC_tSMP_Msk 0b111

typedef enum{
  CADC_CH0  = 0,
  CADC_CH1  = 1,
  CADC_CH2  = 2,
  CADC_CH3  = 3,
  CADC_CH4  = 4,
  CADC_CH5  = 5,
  CADC_CH6  = 6,
  CADC_CH7  = 7,
  CADC_CH8  = 8,
  CADC_CH9  = 9,
  CADC_CH10 = 10,
  CADC_CH11 = 11,
  CADC_CH12 = 12,
  CADC_CH13 = 13,
  CADC_CH14 = 14,
  CADC_CH15 = 15,
  CADC_CH16 = 16,
  CADC_CH17 = 17,
}CADC_CHx;

typedef enum{
  CADC_tSMP_1Cyc5 = 0,
  CADC_tSMP_7Cyc5 = 1,
  CADC_tSMP_13Cyc5 = 2,
  CADC_tSMP_28Cyc5 = 3,
  CADC_tSMP_41Cyc5 = 4,
  CADC_tSMP_55Cyc5 = 5,
  CADC_tSMP_71Cyc5 = 6,
  CADC_tSMP_239Cyc5 = 7,
}CADC_tSMP;

typedef enum{
  RT_TIM1_CC1  = 0b000,
  RT_TIM1_CC2  = 0b001,
  RT_TIM1_CC3  = 0b010,
  RT_TIM2_CC2  = 0b011,
  RT_TIM3_TRGO = 0b100,
  RT_TIM4_CC4  = 0b101,
  RT_EXTI11    = 0b110,
  RT_SWSTART   = 0b111
}CADC_RegTrigSrc;

typedef enum{
  IT_TIM1_TRGO  = 0b000,
  IT_TIM1_CC4   = 0b001,
  IT_TIM2_TRGO  = 0b010,
  IT_TIM2_CC1   = 0b011,
  IT_TIM3_CC4   = 0b100,
  IT_TIM4_TRGO  = 0b101,
  IT_EXTI15     = 0b110,
  IT_SWSTART    = 0b111
}CADC_InjTrigSrc;

#pragma pack(1)
typedef struct{
  CADC_CHx CHx:5;   //ADC_CHANNEL_xx
  CADC_tSMP tSMP:3;  //ADC_SAMPLETIME_1CYCLES5
}ADC_aSamp;

typedef struct{
  __IO bool AWD:1;
  __IO bool EOC:1;
  __IO bool IEOC:1;
  __IO bool ISTRT:1;
  __IO bool RSTRT:1;
  __IO unsigned int resv1:3;
  __IO u8 resv2[3];
}CADC_STATR;

typedef struct{
  unsigned int AWDCH:5;
  bool EOCIE:1;    //使能（规则或注入通道）转换结束中断
  bool AWDIE:1;    //使能模拟看门狗中断
  bool IEOCIE:1;   //使能注入通道组转换结束中断
  bool SCAN:1;     //使能扫描模式
  bool AWDSGL:1;   //扫描模式下，模拟看门狗是否单一通道
  bool IAUTO:1;    //规则通道后转换注入通道
  bool RDISCEN:1;  //使能规则通道间断模式
  bool IDISCEN:1;  //使能注入通道间断模式
  unsigned int DISCNUM:3;
  unsigned int resv1:6;
  bool IAWDEN:1;   //使能注入通道模拟看门狗
  bool RAWDEN:1;   //使能规则通道模拟狗
  bool TKENABLE:1; //使能TKEY触摸键
  unsigned int resv2:7;
}CADC_CTLR1;

typedef struct{
  bool ADON:1;     //ADC开启
  bool CONT:1;     //连续转换使能
  bool CAL:1;      //校准
  bool RSTCAL:1;   //复位校准
  unsigned int resv1:4;
  bool DMA:1;
  unsigned int resv2:2;
  bool ALIGN:1;
  CADC_InjTrigSrc IEXTSEL:3;
  bool IEXTTRIG:1;
  unsigned int resv3:1;
  CADC_RegTrigSrc REXTSEL:3;
  bool REXTTRIG:1;
  bool ISWSTART:1;
  bool RSWSTART:1;
  bool TSVREFE:1;
  unsigned int resc4:8;
}CADC_CTLR2;
#pragma pack()

#pragma pack(4)
typedef struct{
  __IO CADC_STATR STATR;
  __IO CADC_CTLR1 CTLR1;
  __IO CADC_CTLR2 CTLR2;
  __IO u32 SAMPTR1;
  __IO u32 SAMPTR2;
  __IO u32 IOFRx[4];
  __IO u32 WDHTR;
  __IO u32 WDLTR;
  __IO u32 RSQRx[3];
  __IO u32 ISQR;
  __IO u32 IDATAx[4];
  __IO u32 RDATAR;
}CADC_Regs;
#pragma pack()

class C_ADC : public CADC_Regs{
public:
  C_ADC(ADC_TypeDef *regs);
  void Init();
  void Init_DMA();
  void conn_tim(C_TIM *ctim);
  void set_rTrig();
  void set_tSMP(CADC_CHx channel, CADC_tSMP smp);
  u16  read_channel(CADC_CHx channel, CADC_tSMP smp);
  void DMA_once(u16* buffer, u16 Nsamp);
  void Load_Reg_Seq(const ADC_aSamp *smps, u32 len);
  void Load_Inj_Seq(const ADC_aSamp *smps, u32 len);
};

#endif /* __C_ADC_HPP__ */
