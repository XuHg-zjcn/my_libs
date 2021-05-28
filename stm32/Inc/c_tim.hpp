/*
 * timer.h
 *
 *  Created on: Apr 17, 2021
 *      Author: xrj
 */

#ifndef INC_STM32_TIM_HPP_
#define INC_STM32_TIM_HPP_

#include "myints.h"
#include "mylibs_config.hpp"

#define CompKeep 0xffffffff  //flag for `set_comp4()` keep old compare value.
#define DutyKeep -1          //flag for `set_duty4()` keep old compare value.

typedef uint64_t Type_ns;
typedef float    Type_Hz;
typedef uint64_t TypeDiv;

//enums for timer operate
typedef enum{
	TIM_ClkLv_APBx = 0,
	TIM_ClkLv_Mult2,
	TIM_ClkLv_CKD,
	TIM_ClkLv_PSC16b,
	TIM_ClkLv_AutoLoad
}TIM_ClockLevel;

typedef enum{
	TIM_CountMode_Up   = TIM_COUNTERMODE_UP,
	TIM_CountMode_Down = TIM_COUNTERMODE_DOWN,
	TIM_CountMode_Cen1 = TIM_COUNTERMODE_CENTERALIGNED1,
	TIM_CountMode_Cen2 = TIM_COUNTERMODE_CENTERALIGNED2,
	TIM_CountMode_Cen3 = TIM_COUNTERMODE_CENTERALIGNED3
}TIM_CountMode;


//enums for channel operate
typedef enum{
	TIM_Channel_1 = TIM_CHANNEL_1,
	TIM_Channel_2 = TIM_CHANNEL_2,
	TIM_Channel_3 = TIM_CHANNEL_3,
	TIM_Channel_4 = TIM_CHANNEL_4
}TIM_CHx;

typedef enum{
	TIM_CCx_Enable = TIM_CCx_ENABLE,
	TIM_CCx_Disable = TIM_CCx_DISABLE
}TIM_CCxE;

typedef enum{
	TIM_OCMode_Timing          = TIM_OCMODE_TIMING,
	TIM_OCMode_Active          = TIM_OCMODE_ACTIVE,
	TIM_OCMode_Inactive        = TIM_OCMODE_INACTIVE,
	TIM_OCMode_Toggle          = TIM_OCMODE_TOGGLE,
	TIM_OCMode_PWM1            = TIM_OCMODE_PWM1,
	TIM_OCMode_PWM2            = TIM_OCMODE_PWM2,
	TIM_OCMode_Forced_Active   = TIM_OCMODE_FORCED_ACTIVE,
	TIM_OCMode_Forced_InActive = TIM_OCMODE_FORCED_INACTIVE
}TIM_OCMode;


class C_TIM : public TIM_HandleTypeDef{
public:
	//Trig and Clock
	void set_Trig(u32 trig, u32 polar, u32 prescale, u32 filter);
	void set_ExtClk(bool isExtern);
	//low level API get/set ClockDiv(CKD), PreScale(PSC), AutoLoad(ARR)
	inline u32 get_clockdiv_sft();
	inline u32 get_clockdiv_2n();
	inline u32 get_prescale();
	inline u32 get_autoload();
	inline void set_clockdiv_sft(u32 ckd);
	inline void set_clockdiv_2n(u32 ckd_2n);
	inline void set_prescale(u32 psc);
	inline void set_autoload(u32 arr);
	//high level API get/set ClockDiv(CKD), PreScale(PSC), AutoLoad(ARR)
	TypeDiv get_div(TIM_ClockLevel level);
	Type_Hz get_Hz(TIM_ClockLevel level);
	Type_ns get_ns(TIM_ClockLevel level);
	TypeDiv set_tdiv(TypeDiv div);
	void set_Hz(Type_Hz Hz);
	void set_ns(Type_ns ns);
	//others
	void set_CountEnable(bool isEnable);
	inline void set_CountMode(TIM_CountMode mode);
	inline void reset_count();
	u32 maxcount();

	//operate single channel
	inline u32   get_comp(TIM_CHx Channel);
	inline float get_duty(TIM_CHx Channel);
	inline void set_comp(TIM_CHx Channel, u32 comp);
	inline void set_duty(TIM_CHx Channel, float duty);
	//operate 4 channels
	void set_comp4(u32 comp1, u32 comp2, u32 comp3, u32 comp4);
	void set_duty4(float duty1, float duty2, float duty3, float duty4);
	//OnePluse Mode
	void pluse_clk(TIM_CHx Channel, u32 delay_clk, u32 pluse_clk, bool blocking);
	void pluse_ns(TIM_CHx Channel, u32 delay_ns, u32 pluse_ns, bool blocking);
	//others
	void set_OCMode(TIM_CHx Channel, TIM_OCMode mode);
	void CCxChannelCmd(TIM_CHx Channel, TIM_CCxE ChannelState);
};


//low level API get/set ClockDiv(CKD), PreScale(PSC), AutoLoad(ARR)

//get bit masked clock div value
inline u32 C_TIM::get_clockdiv_sft()
{
	return __HAL_TIM_GET_CLOCKDIVISION(this);
}

//get 2^n clock div value
inline u32 C_TIM::get_clockdiv_2n()
{
	return __HAL_TIM_GET_CLOCKDIVISION(this) >> TIM_CR1_CKD_Pos;
}

inline u32 C_TIM::get_prescale()
{
	return READ_REG(this->Instance->PSC);
}

inline u32 C_TIM::get_autoload()
{
	return READ_REG(this->Instance->ARR);
}

inline void C_TIM::set_clockdiv_sft(u32 ckd)
{
	__HAL_TIM_SET_CLOCKDIVISION(this, ckd);
}


inline void C_TIM::set_clockdiv_2n(u32 ckd_2n)
{
	__HAL_TIM_SET_CLOCKDIVISION(this, ckd_2n << TIM_CR1_CKD_Pos);
}

inline void C_TIM::set_prescale(u32 psc)
{
	WRITE_REG(this->Instance->PSC, psc);
}

inline void C_TIM::set_autoload(u32 arr)
{
	WRITE_REG(this->Instance->ARR, arr);
}


/*
 * set count mode.
 * @param mode: can be TIM_CountMode_(Up/Down/Cen1/Cen2/Cen3)
 */
inline void C_TIM::set_CountMode(TIM_CountMode mode)
{
	MODIFY_REG(this->Instance->CR1, TIM_CR1_DIR|TIM_CR1_CMS, mode);
}

//reset count value, upcounter set to 0, downcounter set to autoload value.
inline void C_TIM::reset_count()
{
	uint32_t cnt = __HAL_TIM_IS_TIM_COUNTING_DOWN(this)?__HAL_TIM_GET_AUTORELOAD(this):0;
	__HAL_TIM_SET_COUNTER(this, cnt);
}



//operate single channels

//get single channel compare value
inline u32 C_TIM::get_comp(TIM_CHx Channel)
{
	return __HAL_TIM_GET_COMPARE(this, Channel);
}

//get single channel duty 0..1
inline float C_TIM::get_duty(TIM_CHx Channel)
{
	uint32_t al = __HAL_TIM_GET_AUTORELOAD(this);
	return (float)__HAL_TIM_GET_COMPARE(this, Channel)/al;
}

//set single channel compare value
inline void C_TIM::set_comp(TIM_CHx Channel, u32 comp)
{
	__HAL_TIM_SET_COMPARE(this, Channel, comp);
}

//set single channel compare value by duty 0..1
inline void C_TIM::set_duty(TIM_CHx Channel, float duty)
{
	uint32_t al = __HAL_TIM_GET_AUTORELOAD(this);
	__HAL_TIM_SET_COMPARE(this, Channel, duty*al);
}

#endif /* INC_TIMER_HPP_ */
