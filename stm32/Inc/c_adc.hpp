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
#include "buffer.hpp"
#include "myints.h"
#include "mylibs_config.hpp"

const uint32_t tSMPs[8] = {1, 7, 13, 28, 41, 55, 71, 239};
#define T_SAMP2CLKS(smp) (tSMPs[(smp)>>ADC_SMPR1_SMP10_Pos])
#define REF_NSAMP  9
#ifndef USE_FLOAT
#define MV_MUL    10    //multiply to result, can improve accuracy, too big maybe overflow!
#endif

/*
 * how use the driver:
 * STM32CubeMX config ADC channel, DMA
 *
 * add code in global
 *    extern ADC_HandleTypeDef hadc1;
 *    extern TIM_HandleTypeDef htim3;
 *    C_ADCEx adc = C_ADCEx(&hadc1);
 *    Buffer buff = Buffer(2);
 *
 * add callback code
 *     void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
 *     {
 *         eadc.ConvCplt();
 *     }
 *
 *     void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
 *     {
 *         eadc.ConvHalfCplt();
 *     }
 *
 * add init code after FreeRTOS Start scheduler
 *     buff.Init();
 *     buff.remalloc(128);
 *     adc.Init();
 *     adc.conn_buff(&buff.w_head);
 *     adc.conn_tim(&htim3, TIM_Channel_1);
 *     adc.set_SR_sps(10000);
 *     adc.load_regular_one_channel(TIM_Channel_1, ADC_tSMP_28Cyc5); //optional
 *
 * add user code
 *     u16* p;
 *     int head_i = buff.r_heads.new_head();
 *     while(1){
 *         adc.dma_once();
 *         p = buff.r_heads.get_frames(head_i, 128);
 *         //add your code, data process
 *     }
 */

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

typedef enum{
	ADC_CH1  = ADC_CHANNEL_1,
	ADC_CH2  = ADC_CHANNEL_2,
	ADC_CH3  = ADC_CHANNEL_3,
	ADC_CH4  = ADC_CHANNEL_4,
	ADC_CH5  = ADC_CHANNEL_5,
	ADC_CH6  = ADC_CHANNEL_6,
	ADC_CH7  = ADC_CHANNEL_7,
	ADC_CH8  = ADC_CHANNEL_8,
	ADC_CH9  = ADC_CHANNEL_9,
	ADC_CH10 = ADC_CHANNEL_10,
	ADC_CH11 = ADC_CHANNEL_11,
	ADC_CH12 = ADC_CHANNEL_12,
	ADC_CH13 = ADC_CHANNEL_13,
	ADC_CH14 = ADC_CHANNEL_14,
	ADC_CH15 = ADC_CHANNEL_15,
	ADC_CH16 = ADC_CHANNEL_16,
	ADC_CH17 = ADC_CHANNEL_17
}ADC_CHx;

typedef enum{
	ADC_tSMP_1Cyc5   = ADC_SAMPLETIME_1CYCLE_5,
	ADC_tSMP_7Cyc5   = ADC_SAMPLETIME_7CYCLES_5,
	ADC_tSMP_13Cyc5  = ADC_SAMPLETIME_13CYCLES_5,
	ADC_tSMP_28Cyc5  = ADC_SAMPLETIME_28CYCLES_5,
	ADC_tSMP_41Cyc5  = ADC_SAMPLETIME_41CYCLES_5,
	ADC_tSMP_55Cyc5  = ADC_SAMPLETIME_55CYCLES_5,
	ADC_tSMP_71Cyc5  = ADC_SAMPLETIME_71CYCLES_5,
	ADC_tSMP_239Cyc5 = ADC_SAMPLETIME_239CYCLES_5
}ADC_tSMP;

#pragma pack(1)
/*
 * sample time register has each CHANNEL in ADC_SMPRx (x=1,2)
 * can't set different sample time to same channel.
 */
typedef struct{
	ADC_CHx CHx:5;   //ADC_CHANNEL_xx
	ADC_tSMP tSMP:3;  //ADC_SAMPLETIME_1CYCLES5
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

typedef union{
	MyADCModeStruct Stru;
	MyADCModeEnum Enum;
}MyADCMode;

class C_ADC : public ADC_HandleTypeDef{
public:
};

class C_ADCEx{
protected:
	C_ADC *hadc;
	C_TIM *htim;
	TIM_CHx chx;
	BuffHeadWrite *w_head;
	MyADCMode mode;
	uint32_t timeout;
	uint32_t NDTR;
	uint32_t Xref; //1.2V基准电压, REF_NSAMP次采样求和
public:
	C_ADCEx(ADC_HandleTypeDef *hadc);
	void Init();
	void conn_tim(TIM_HandleTypeDef *htim, TIM_CHx channel);
	void conn_buff(BuffHeadWrite* w_head);  //TODO: auto create buffer
	void set_SR_sps(u32 sps);
	void set_SR_ns(u32 ns);
	void set_Inject_ExtenTrig(u32 src, u32 edge);
	void set_Regular_ExtenTrig(u32 src, u32 edge);
	void load_regular_seq(ADC_SampSeq* sseq);
	void load_inject_seq(ADC_SampSeq* sseq);
	void load_regular_one_channel(ADC_CHx CHx, ADC_tSMP tSAMP);
	void Injected_once(bool blocking);
	void Regular_once(u16 *buf, bool blocking);
	void DMA_once(u32 Nsamp, bool blocking);
	void DMA_cycle(u32 cycle);
	void ConvCplt();      //please call in DMA Conv callback
	void ConvHalfCplt();  //please call in DMA ConvHalf callback
	void ConvPack();
	uint32_t read_channel_sum(ADC_CHx channel, ADC_tSMP sample_time, u32 n);
	//get voltage, base ADC_CH17 1.2V ref
	uint16_t update_ref();
#ifdef USE_FLOAT
	float read_Volt(ADC_CHx channel, ADC_tSMP sample_time, u32 n);
	float Vdd_Volt();
#else
	uint16_t read_mV(ADC_CHx channel, ADC_tSMP sample_time, u32 n);
	uint16_t Vdd_mV();
#endif
};

//TODO: single channel API

#endif /* INC_MY_ADC_HPP_ */
