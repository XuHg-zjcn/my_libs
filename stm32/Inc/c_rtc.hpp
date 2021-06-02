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

using namespace std;


class C_RTC : public RTC_HandleTypeDef{
public:
	static void HAL2tm(struct tm* tm2, RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);
	static void tm2HAL(struct tm* tm2, RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);
	struct tm* get_tm();
	void set_tm(struct tm *tm2);
	time_t get_timestamp();
	void set_timestamp(time_t ts);
	u32 get_cnt();
	void set_cnt(u32 cnt);
	void BKUPWrite(uint32_t BackupRegister, uint32_t Data);
	u32 BKUPRead(uint32_t BackupRegister);
};


#endif /* STM32_INC_C_RTC_HPP_ */
