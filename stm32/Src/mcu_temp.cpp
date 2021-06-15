/*
 * mcu_temp.cpp
 *
 *  Created on: 2021年6月8日
 *      Author: xrj
 */

#include "mcu_temp.hpp"


#ifdef USE_FLOAT
const TempSenParam tsp={1.34, 232.5, ADC_tSMP_239Cyc5, 9, 0};
#else
const TempSenParam tsp={1340*MV_MUL, 233, ADC_tSMP_239Cyc5, 9, 0};
#endif


MCUTempSen::MCUTempSen(C_ADC *cadc)
{
	this->cadc = cadc;
}

#ifdef USE_FLOAT
float MCUTempSen::read_Volt()
{
	cadc->update_ref();
	return cadc->read_Volt(ADC_CH16, tsp.t_smp, tsp.N);
}

float MCUTempSen::calib_V25(float temp)
{
	float V_sen = read_Volt();
	float V_25 = V_sen + (temp-25.0f)/tsp.oC_V;
	if(V_25<1.2f or V_25>1.6f){
		return 0.0f;
	}return V_25;
}

float MCUTempSen::get_temp()
{
	float V_sen = read_Volt();
	return (tsp.V_25-V_sen)*tsp.oC_V + 25.0f;
}
#else
u16 MCUTempSen::read_mV()
{
	cadc->update_ref();
	return cadc->read_mV(ADC_CH16, tsp.t_smp, tsp.N);
}

u16 MCUTempSen::calib_mV25(i32 temp100)
{
	i32 mV_sen = read_mV();
	i32 mV_25 = mV_sen + (temp100-2500)*10/tsp.oC_V;
	if(mV_25<1200*MV_MUL or mV_25 >1600*MV_MUL){
		return 0;
	}return mV_25;
}

i32 MCUTempSen::get_temp100()
{
	i32 mV_sen = read_mV();
	return ((i32)tsp.mV_25-mV_sen)*(i32)tsp.oC_V/(10*MV_MUL) + 2500;
}
#endif

void MCUTempSen::save(TempSenParam *param)
{
	//TODO: flash write
}

