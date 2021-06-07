/*
 * c_pwr.cpp
 *
 *  Created on: Jun 7, 2021
 *      Author: xrj
 */

#include "c_pwr.hpp"

/*
 * note: please call SystemClock_Config() after wake-up from STOP mode
 *       STOP mode test failed.
 */
void C_PWR::EntryMode(PWR_Mode mode, PWR_WFx wfx)
{
    __HAL_RCC_PWR_CLK_ENABLE();
    PWR->CR |= 1<<2;           //清除Wake-up 标志
    PWR->CR |= 1<<3;           //清除待机 标志
	switch(mode){
	case PWR_Standby:
		HAL_PWR_EnterSTANDBYMode();
		break;
	case PWR_Stop:
		HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, wfx);
		break;
	case PWR_Sleep:
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, wfx);
		break;
	default:
		break;
	}
}

void C_PWR::ConfigPVD(PVD_Level level, PVD_Mode mode)
{
	PWR_PVDTypeDef sConfigPVD = {level, mode};
	HAL_PWR_ConfigPVD(&sConfigPVD);
}
