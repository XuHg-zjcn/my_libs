/*
 * my_adc.hpp
 *
 *  Created on: 2021年4月25日
 *      Author: xrj
 */

#ifndef INC_MY_ADC_HPP_
#define INC_MY_ADC_HPP_

#include "c_adc.hpp"
#include "c_tim.hpp"
#include "buffer.hpp"
#include "myints.h"

class C_ADC{
public:
	uint16_t read_channel(uint32_t channel, uint32_t sample_time);
}


class C_ADCEx{
protected:
	C_ADC *hadc;
	C_TIM *htim;
	BuffHeadWrite *w_head;
	MyADCMode mode;
public:
	C_ADCEx();
	void Init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim);
	void conn_buff(BuffHeadWrite *w_head);
	void set_SR_sps(u32 sps);
	void set_SR_ns(u32 ns);
	//void DMA_once(u32 Nsamp, bool blocking);
	//void DMA_cycle(u32 cycle);
	//void ConvCplt();      //please call in DMA Conv callback
	//void ConvHalfCplt();  //please call in DMA ConvHalf callback
	//void ConvPack();
};

#endif /* INC_MY_ADC_HPP_ */
