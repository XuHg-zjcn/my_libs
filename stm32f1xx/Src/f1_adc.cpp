#include "f1_adc.hpp"


/*
  tSAMP = 1.5      : ADC_DUALMODE_INTERLFAST
  7.5<=tSAMP<=13.5 : ADC_DUALMODE_INTERLSLOW
  tSAMP >= 28.5    : entry Error_Handler()
*/
void F1_ADC::ConfigFastMode(u32 CHx, u32 tSAMP, u32 nADC)
{
	ADC_MultiModeTypeDef multimode;
	uint32_t samp_clks = T_SAMP2CLKS(tSAMP) + 13;
	if(nADC <1 || nADC > TOTAL_ADCs){
		Error_Handler();
	}if(tSAMP > ADC_SAMPLETIME_13CYCLES_5){
		Error_Handler();
	}
	//config channal
	load_regular_one_channel(CHx, tSAMP);
	//regs
	CLEAR_BIT(hadc->Instance->CR1, ADC_CR1_SCAN);
	CLEAR_BIT(hadc->Instance->CR1, ADC_CR1_DISCEN);
	SET_BIT(hadc->Instance->CR2, ADC_CR2_CONT);
	//config MultiADC Mode
	if(nADC>1){
		multimode.Mode = (tSAMP==ADC_SAMPLETIME_1CYCLE_5) ? ADC_DUALMODE_INTERL : ADC_DUALMODE_INTERLSLOW;
		HAL_ADCEx_MultiModeConfigChannel(hadc, &multimode);
	}
}
