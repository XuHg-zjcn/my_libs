#include "stm32_adc.hpp"
#include "stm32f4xx_hal.h"

const uint32_t tSMPs[8] = {3, 15, 28, 56, 84, 112, 144, 480};

#define TOTAL_ADCs 3

class F4_ADC : public STM32_ADC{
public:
	void ConfigFastMode(u32 CHx, u32 tSAMP, u32 res, u32 nADC);
}
