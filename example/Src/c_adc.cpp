/*
 * my_adc.cpp
 *
 *  Created on: 2021年4月25日
 *      Author: xrj
 */

#include "c_adc.hpp"

#include "myints.h"
#include "c_tim.hpp"



C_ADCEx::C_ADCEx()
{
	this->htim = nullptr;
	this->hadc = nullptr;
	this->w_head = nullptr;
	this->mode.Enum = ADC_stopping;
}

void C_ADCEx::Init(C_ADC *hadc, C_TIM *htim)
{
    this->hadc = hadc;
    this->htim = htim;
}

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
