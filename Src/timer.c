/*
 * timer.c
 *
 *  Created on: Apr 17, 2021
 *      Author: xrj
 */

#include "stm32f4xx_hal.h"

uint32_t GetTimerFreq(TIM_HandleTypeDef *htim)
{
	uint32_t freq;
	if(((uint32_t)htim->Instance) > APB2PERIPH_BASE){
		freq = HAL_RCC_GetPCLK2Freq();
	}else{
		freq = HAL_RCC_GetPCLK1Freq();
	}
	freq *= 2;
	uint32_t CKD = __HAL_TIM_GET_CLOCKDIVISION(htim) >> TIM_CR1_CKD_Pos;
	freq /= (1 << CKD);
	freq /= (htim->Instance->PSC + 1);
	return freq;
}


/*
 * force output active state.
 */
void TIM_OCMode(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t OCMode)
{
	switch(Channel){
	case TIM_CHANNEL_1:
		MODIFY_REG(htim->Instance->CCMR1, TIM_CCMR1_OC1M, OCMode);
		break;
	case TIM_CHANNEL_2:
		MODIFY_REG(htim->Instance->CCMR1, TIM_CCMR1_OC2M, OCMode<<8);
		break;
	case TIM_CHANNEL_3:
		MODIFY_REG(htim->Instance->CCMR2, TIM_CCMR2_OC3M, OCMode);
		break;
	case TIM_CHANNEL_4:
		MODIFY_REG(htim->Instance->CCMR2, TIM_CCMR2_OC4M, OCMode<<8);
		break;
	default:
		break;
	}
}
