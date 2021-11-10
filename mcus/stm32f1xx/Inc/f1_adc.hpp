#include "c_adc.hpp"

#if !defined(F1_ADC_HPP_) && defined(INC_C_ADC_HPP_)
#define F1_ADC_HPP_

#include "stm32f1xx_hal.h"
//TODO: rename F1_ADC->C_ADC, C_ADC->STM32_ADC
class F1_ADC : public C_ADC{
public:
	void ConfigFastMode(ADC_CHx CHx, ADC_tSMP tSAMP, u32 nADC);
};
#endif
