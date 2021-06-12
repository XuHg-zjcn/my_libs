#ifndef F1_ADC_HPP_
#define F1_ADC_HPP_

#include "c_adc.hpp"
#include "stm32f1xx_hal.h"


class F1_ADCEx : public C_ADCEx{
public:
	void ConfigFastMode(ADC_CHx CHx, ADC_tSMP tSAMP, u32 nADC);
};

#endif
