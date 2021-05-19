/*
 * my_adc.hpp
 *
 *  Created on: 2021年4月25日
 *      Author: xrj
 */

#ifndef INC_C_ADC_HPP_
#define INC_C_ADC_HPP_

#include "c_adc.hpp"
#include "c_tim.hpp"
#include "cmsis_os2.h"
#include "buffer.hpp"
#include "myints.h"
#include "mylibs_config.h"

extern const uint32_t tSMPs[8];
#define T_SAMP2CLKS(smp) (tSMPs[(smp)>>ADC_SMPR1_SMP10_Pos])

#pragma pack(1)
/*
 * sample time register has each CHANNEL in ADC_SMPRx (x=1,2)
 * can't set different sample time to same channel.
 */
typedef struct{
	unsigned int CHx:5;   //ADC_CHANNEL_xx
	unsigned int tSMP:3;  //ADC_SAMPLETIME_xxCYCLES
}ADC_aSamp;

typedef struct{
	uint8_t len;    //length of sequence-1
	ADC_aSamp *seq;
}ADC_SampSeq;
#pragma pack()

typedef struct{
	unsigned int running:1;   // 0:stopping, 1:running
	unsigned int inject:1;    // 0:regular,  1:injected
	unsigned int contin:2;    // 0:sample one, 1:scan once, 2:DMA once, 3:DMA multiply
	unsigned int sem_full:1;  // sem release on full cpkt
	unsigned int sem_half:1;  // sem release on half cpkt
	unsigned int sem_pack:1;
}MyADCModeStruct;

typedef enum{              // |PHFCCIR,
	ADC_stopping           = 0b0000000,
	ADC_injected           = 0b0000011,
	ADC_regular_single     = 0b0000001,
	ADC_regular_scan       = 0b0010101,
	ADC_dma_once_Blocking  = 0b0011001,
	ADC_dma_once_NoBlock   = 0b0001001,
	ADC_dma_cont_HalfFull  = 0b0111101,
	ADC_dma_cont_Multi     = 0b1001101,
}MyADCModeEnum;

typedef union{
	MyADCModeStruct Stru;
	MyADCModeEnum Enum;
}MyADCMode;

class C_ADC : public ADC_HandleTypeDef{
public:
	u16 read_channel(u32 channel, u32 sample_time);
};

class C_ADCEx{
protected:
	C_ADC *hadc;
	C_TIM *htim;
	BuffHeadWrite *w_head;
	MyADCMode mode;
public:
	C_ADCEx();
	void Init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim);
	void set_Inject_ExtenTrig(u32 src, u32 edge);
	void set_Regular_ExtenTrig(u32 src, u32 edge);
	void load_regular_seq(ADC_SampSeq* sseq);
	void load_inject_seq(ADC_SampSeq* sseq);
	void load_regular_one_channel(u32 CHx, u32 tSAMP);
	void Injected_once(bool blocking);
	void Regular_once(u16 *buf, bool blocking);
	void DMA_once(u32 Nsamp, bool blocking);
	void DMA_cycle(u32 cycle);
	void ConvCplt();      //please call in DMA Conv callback
	void ConvHalfCplt();  //please call in DMA ConvHalf callback
	void ConvPack();
};

#endif /* INC_MY_ADC_HPP_ */