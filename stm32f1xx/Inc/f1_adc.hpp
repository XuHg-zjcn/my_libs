#ifndef F1_ADC_HPP_
#define F1_ADC_HPP_

#include "c_adc.hpp"
#include "stm32f1xx_hal.h"

const uint32_t tSMPs[8] = {1, 7, 13, 28, 41, 55, 71, 239};

#define TOTAL_ADCs 2

class F1_ADC : public C_ADC{
public:
	void ConfigFastMode(u32 CHx, u32 tSAMP, u32 nADC);
};

#endif
