#include "f4_adc.hpp"

#define ADCRES2CLKS(res)  (15-(res>>ADC_CR1_RES_Pos)*2)

/*
 * (sample_time + convert time)*nADC <= 20
 */
void F4_ADC::ConfigFastMode(u32 CHx, u32 tSAMP, u32 res, u32 nADC)
{
	ADC_MultiModeTypeDef multimode;
	uint32_t samp_clks = T_SAMP2CLKS(tSAMP) + ADCRES2CLKS(res);
	if(nADC <1 || nADC > TOTAL_ADCs){
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
		multimode.Mode = (nADC==2)?ADC_DUALMODE_INTERL:ADC_TRIPLEMODE_INTERL;
		multimode.DMAAccessMode = ADC_DMAACCESSMODE_2;  //2 / 3 half-words by pairs - 2&1 then 1&3 then 3&2
		multimode.TwoSamplingDelay = samp_clks/nADC;
		if(multimode.TwoSamplingDelay > 20){
			Error_Handler();
		}
		multimode.TwoSamplingDelay -= 5;
		multimode.TwoSamplingDelay <<= ADC_CCR_DELAY_Pos;
		HAL_ADCEx_MultiModeConfigChannel(hadc, &multimode);
	}
}
