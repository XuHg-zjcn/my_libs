/*
 * c_rtc.hpp
 *
 *  Created on: Jun 1, 2021
 *      Author: xrj
 */

#ifndef STM32_INC_C_RTC_HPP_
#define STM32_INC_C_RTC_HPP_

#include <ctime>
#include "mylibs_config.hpp"
#include "myints.h"
#include "x_base.hpp"

using namespace std;

#define TIMESTAMP1970_BIAS 946684800  //2000.1.1

//not suggest use HAL RTC, it will lost date when reboot!
//TODO: cache date
class C_RTC : public RTC_HandleTypeDef{
private:
	X_State EnterInitMode();
	X_State ExitInitMode();
public:
	u32     ReadTimeCounter();
	X_State WriteTimeCounter(uint32_t TimeCounter);
	u32     ReadAlarmCounter();
	X_State WriteAlarmCounter(uint32_t AlarmCounter);
	u16     ReadDIVL();
	time_t  get_ts1970();
	X_State set_ts1970(time_t ts);
	struct tm* get_tm();
	X_State    set_tm(struct tm *tm2);
#ifdef USE_HAL_TIM
	void GetTime(RTC_TimeTypeDef *sTime, u32 Format) {HAL_RTC_GetTime(this, sTime, Format);}
	void GetDate(RTC_DateTypeDef *sDate, u32 Format) {HAL_RTC_GetDate(this, sDate, Format);}
	void SetAlarm(RTC_TimeTypeDef *sTime, u32 Format, bool IT);
	void GetAlarm(RTC_TimeTypeDef *sTime, uint32_t Format);
#endif
	//IT operate: RTC_IT_OW, RTC_IT_ALRA, RTC_IT_SEC, RTC_IT_TAMP1
	inline void EnableIT(u32 IT)                     {__HAL_RTC_ALARM_ENABLE_IT(this, IT);};
	inline void DisableIT(u32 IT)                    {__HAL_RTC_ALARM_DISABLE_IT(this, IT);};
	inline ITStatus GetIT(u32 IT)                    {return __HAL_RTC_ALARM_GET_IT_SOURCE(this, IT);};
	//backup register operate
	void BKUPWrite(u32 BackupRegister, u32 Data)     {HAL_RTCEx_BKUPWrite(this, BackupRegister, Data);};
	u32 BKUPRead(u32 BackupRegister)                 {return HAL_RTCEx_BKUPRead(this, BackupRegister);};
};


#endif /* STM32_INC_C_RTC_HPP_ */
