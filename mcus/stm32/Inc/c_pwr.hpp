/*
 * c_pwr.hpp
 *
 *  Created on: Jun 7, 2021
 *      Author: xrj
 */

#ifndef STM32_INC_C_PWR_HPP_
#define STM32_INC_C_PWR_HPP_

#include "mylibs_config.hpp"


typedef enum{         // | CPU  | RAM  | perh | RTC | current
	PWR_Standby = 0,  // | stop | lost | stop | run |   x uA
	PWR_Stop,         // | stop | keep | stop | run |  xx uA
	PWR_Sleep,        // | stop | keep |  run | run |   x mA
	PWR_Run,          // |  run | keep |  run | run |  xx mA
}PWR_Mode;

typedef enum{
	PWR_WFI = PWR_STOPENTRY_WFI,
	PWR_WFE = PWR_STOPENTRY_WFE
}PWR_WFx;


typedef enum{
	PVD_2V2 = PWR_CR_PLS_2V2,
	PVD_2V3 = PWR_CR_PLS_2V3,
	PVD_2V4 = PWR_CR_PLS_2V4,
	PVD_2V5 = PWR_CR_PLS_2V5,
	PVD_2V6 = PWR_CR_PLS_2V6,
	PVD_2V7 = PWR_CR_PLS_2V7,
	PVD_2V8 = PWR_CR_PLS_2V8,
	PVD_2V9 = PWR_CR_PLS_2V9,
}PVD_Level;

typedef enum{
	PVD_Normal      = PWR_PVD_MODE_NORMAL,
	PVD_IT_Ris      = PWR_PVD_MODE_IT_RISING,
	PVD_IT_Fall     = PWR_PVD_MODE_IT_FALLING,
	PVD_IT_RisFall  = PWR_PVD_MODE_IT_RISING_FALLING,
	PVD_EVT_Ris     = PWR_PVD_MODE_EVENT_RISING,
	PVD_EVT_Fall    = PWR_PVD_MODE_EVENT_FALLING,
	PVD_EVT_RisFall = PWR_PVD_MODE_EVENT_RISING_FALLING,
}PVD_Mode;

class C_PWR{
public:
	void EntryMode(PWR_Mode mode, PWR_WFx wfx);
	void ConfigPVD(PVD_Level level, PVD_Mode mode);
};


#endif /* STM32_INC_C_PWR_HPP_ */
