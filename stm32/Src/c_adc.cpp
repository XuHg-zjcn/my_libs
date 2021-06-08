/*
 * my_adc.cpp
 *
 *  Created on: 2021年4月25日
 *      Author: xrj
 */

#include "c_adc.hpp"
#include "c_tim.hpp"

#include <stdlib.h>
#include "main.h"
#include "ops.hpp"
#include "mylibs_config.hpp"

//replace for STM32F1
#define ADC_CR2_EXTEN ADC_CR2_EXTTRIG
#define ADC_CR2_JEXTEN ADC_CR2_JEXTTRIG


C_ADCEx::C_ADCEx()
{
	this->htim = nullptr;
	this->hadc = nullptr;
	this->w_head = nullptr;
	this->mode.Enum = ADC_stopping;
	this->timeout = 1000;
	this->NDTR = 0;
}

void C_ADCEx::Init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim)
{
    this->hadc = (C_ADC*)hadc;
    this->htim = (C_TIM*)htim;
}

//TODO: change to Buffer ptr
void C_ADCEx::conn_buff(BuffHeadWrite* w_head)
{
       this->w_head = w_head;
}

void C_ADCEx::set_SR_sps(u32 sps)
{
       htim->set_Hz(sps);
}

void C_ADCEx::set_SR_ns(u32 ns)
{
       htim->set_ns(ns);
}

/*
 * @param src: ADC_CR2_JEXTSEL  @ref ADCEx_External_trigger_Source_Injected
 * @param edge: ADC_CR2_JEXTEN  @ref ADCEx_External_trigger_edge_Injected
 */
void C_ADCEx::set_Inject_ExtenTrig(u32 src, u32 edge)
{
	MODIFY_REG(hadc->Instance->CR2, ADC_CR2_JEXTSEL, src);
	MODIFY_REG(hadc->Instance->CR2, ADC_CR2_JEXTEN, edge);
}

/*
 * @param src: ADC_CR2_EXTSEL  @ref ADC_External_trigger_Source_Regular
 * @param edge: ADC_CR2_EXTEN  @ref ADC_External_trigger_edge_Regular
 */
void C_ADCEx::set_Regular_ExtenTrig(u32 src, u32 edge)
{
	MODIFY_REG(hadc->Instance->CR2, ADC_CR2_EXTSEL, src);
	MODIFY_REG(hadc->Instance->CR2, ADC_CR2_EXTEN, edge);
}

void C_ADCEx::load_regular_seq(ADC_SampSeq* sseq)
{
	ADC_aSamp *seq = sseq->seq;
	int len2 = value_upper(sseq->len, 15);
	MODIFY_REG(hadc->Instance->SQR1, ADC_SQR1_L, len2<<ADC_SQR1_L_Pos);

	ADC_ChannelConfTypeDef sConfig = {0};
	for(int i=0;i<=len2;i++){
		sConfig.Rank = i+1;
		sConfig.Channel = seq->CHx;
		sConfig.SamplingTime = seq->tSMP;
		HAL_ADC_ConfigChannel(hadc, &sConfig);
		seq++;
	}
}

void C_ADCEx::load_inject_seq(ADC_SampSeq* sseq)
{
	ADC_aSamp *seq = sseq->seq;
	int len2 = value_upper(sseq->len, 3);
	MODIFY_REG(hadc->Instance->JSQR, ADC_JSQR_JL, len2<<ADC_JSQR_JL_Pos);

	ADC_InjectionConfTypeDef sConfigInjected = {0};
	for(int i=0;i<=len2;i++){
		sConfigInjected.InjectedRank = i+1;
		sConfigInjected.InjectedChannel = seq->CHx;
		sConfigInjected.InjectedSamplingTime = seq->tSMP;
		HAL_ADCEx_InjectedConfigChannel(hadc, &sConfigInjected);
		seq++;
	}
}

void C_ADCEx::load_regular_one_channel(ADC_CHx CHx, u32 tSAMP)
{
	CLEAR_BIT(hadc->Instance->SQR1, ADC_SQR1_L);  //length=1
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Rank = 1;
	sConfig.Channel = CHx;
	sConfig.SamplingTime = tSAMP;
	HAL_ADC_ConfigChannel(hadc, &sConfig);
}

/*
 * convert injected sequence once
 */
void C_ADCEx::Injected_once(bool blocking)
{
	mode.Enum = ADC_injected;
	SET_BIT(hadc->Instance->CR1, ADC_CR1_SCAN);
	CLEAR_BIT(hadc->Instance->CR1, ADC_CR1_JDISCEN);
	HAL_ADCEx_InjectedStart(hadc);
	if(blocking){
		while(!READ_BIT(hadc->Instance->SR, ADC_SR_JEOC));
	}
}

/*
 * convert regular sequence once
 */
void C_ADCEx::Regular_once(u16 *buf, bool blocking)
{
	mode.Enum = ADC_regular_scan;
	SET_BIT(hadc->Instance->CR1, ADC_CR1_SCAN);
	CLEAR_BIT(hadc->Instance->CR1, ADC_CR1_DISCEN);
	uint32_t length = READ_BIT(hadc->Instance->SQR1, ADC_SQR1_L) >> ADC_SQR1_L_Pos;
	HAL_ADC_Start_DMA(hadc, (uint32_t*)buf, length);
	if(blocking){
		w_head->wait_lock();
	}
}

/*
 * DMA transfer once.
 * @param Nsamp: number of sample, 0:whole buffer,
 *               advice times by sample sequence length.
 * @param blocking: 1:blocking until finish.
 */
void C_ADCEx::DMA_once(u32 Nsamp, bool blocking)
{
	if(!w_head){
		return;
	}
	mode.Enum = blocking ? ADC_dma_once_Blocking : ADC_dma_once_NoBlock;
#ifdef ADC_CR2_DDS
	CLEAR_BIT(hadc->Instance->CR2, ADC_CR2_DDS);
#endif
	u16 *p = (u16*)w_head->put_dma_once(Nsamp);
	if(!p){  //get pointer failed
		return;
	}
	HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
	NDTR = Nsamp;
	hadc->DMA_Handle->Init.Mode = DMA_NORMAL;
	HAL_DMA_Init(hadc->DMA_Handle);
	HAL_ADC_Start_DMA(hadc, (u32*)p, NDTR);
	if(blocking){
		w_head->wait_lock();
	}
}

//TODO: number of cycle
void C_ADCEx::DMA_cycle(u32 cycle)
{
	if(!w_head){
		return;
	}
	mode.Enum = ADC_dma_cont_Multi;
#ifdef ADC_CR2_DDS
	SET_BIT(hadc->Instance->CR2, ADC_CR2_DDS);
#endif
	//TODO: set htim_pack
	u16 *p = (u16*)w_head->put_dma_cycle(cycle);
	if(!p){  //get pointer failed
		return;
	}
	HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
	NDTR = w_head->get_capacity();
	hadc->DMA_Handle->Init.Mode = DMA_CIRCULAR;
	HAL_DMA_Init(hadc->DMA_Handle);
	HAL_ADC_Start_DMA(hadc, (u32*)p, NDTR);
}

//please call in `HAL_ADC_ConvHalfCpltCallback`
void C_ADCEx::ConvHalfCplt()
{
	if(mode.Stru.sem_half){
		w_head->put_dma_notify(NDTR/2);
	}
}

//please call in `HAL_ADC_ConvCpltCallback`
void C_ADCEx::ConvCplt()
{
	if(mode.Stru.sem_full){
		w_head->put_dma_notify(mode.Stru.sem_half ? NDTR/2 : NDTR);
	}if(mode.Stru.contin != 3){
		mode.Enum = ADC_stopping;
		HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_1);
	}
}

//please call in `HAL_TIM_PeriodElapsedCallback` of sample count timer
void C_ADCEx::ConvPack()
{
	if(mode.Stru.sem_pack){
		//TODO: set buff.curr
		w_head->put_dma_notify(NDTR/2);
	}
}

/*
 * use injected length=1 sequence. blocking to wait finish.
 */
uint16_t C_ADCEx::read_channel(ADC_CHx channel, ADC_tSMP sample_time, u32 n)
{
	if(n==0){
		return 0;
	}
	//config inject channel
	ADC_InjectionConfTypeDef sConfigInjected;
	sConfigInjected.ExternalTrigInjecConv = ADC_INJECTED_SOFTWARE_START;
	sConfigInjected.InjectedNbrOfConversion = 1;
	sConfigInjected.InjectedChannel = channel;
	sConfigInjected.InjectedSamplingTime = sample_time;
	sConfigInjected.InjectedRank = ADC_INJECTED_RANK_1;
	HAL_ADCEx_InjectedConfigChannel(hadc, &sConfigInjected);
	//convert n times in blocking mode.
	u32 ret=0;
	for(u32 i=0;i<n;i++){
		HAL_ADCEx_InjectedStart(hadc);
		HAL_ADCEx_InjectedPollForConversion(hadc, timeout);
		ret+=HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
	}
	return ret/n;
}
