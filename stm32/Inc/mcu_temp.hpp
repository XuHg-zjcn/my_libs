/*
 * mcu_temp.hpp
 *
 *  Created on: 2021年6月8日
 *      Author: xrj
 */

#ifndef STM32_INC_MCU_TEMP_HPP_
#define STM32_INC_MCU_TEMP_HPP_

#include "mylibs_config.hpp"
#include "c_adc.hpp"
#include "myints.h"


typedef struct{
#ifdef USE_FLOAT
	float V_25;
	float oC_V;
#else
	uint16_t mV_25;
	uint16_t oC_V;
#endif
	ADC_tSMP t_smp;
	uint32_t N;
	uint32_t ts_calib;
}TempSenParam;

class MCUTempSen{
private:
	C_ADCEx *adc;
public:
	MCUTempSen(C_ADCEx *adc);
#ifdef USE_FLOAT
	float read_Volt();
	float calib_V25(float temp);
	float get_temp();
#else
	u16 read_mV();
	u16 calib_mV25(i32 temp100);
	i32 get_temp100();
#endif
	void save(TempSenParam *param);
};



#endif /* STM32_INC_MCU_TEMP_HPP_ */
