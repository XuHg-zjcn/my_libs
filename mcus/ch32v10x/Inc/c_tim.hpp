/*
 * c_tim.hpp for CH32V103C8T6
 *
 *  Created on: Jan 9, 2022
 *      Author: xrj
 */

#include "ch32v10x_tim.h"
#include "i_pin.hpp"
#if !defined(__C_TIM_HPP__) && \
	defined(__CH32V10x_TIM_H)
	//defined(__I_PIN_HPP__)
#define __C_TIM_HPP__

#include "mylibs_config.hpp"
#include "myints.h"
#include "ops.hpp"

#define N_IT     8           //number interrupts of timer
#define CompKeep 0xffffffff  //flag for `set_comp4()` keep old compare value.
#define DutyKeep -1          //flag for `set_duty4()` keep old compare value.

//TODO: use #include<ratio>
typedef uint32_t Type_ns;
typedef uint32_t Type_Hz;
typedef uint32_t TypeDiv;

//enums for timer operate
typedef enum{
	TIM_ClkLv_APBx = 0,
	TIM_ClkLv_Mult2,
	TIM_ClkLv_CKD,
	TIM_ClkLv_PSC16b,
	TIM_ClkLv_AutoLoad
}TIM_ClockLevel;

/*typedef enum{
	TIM_CountMode_Up   = TIM_COUNTERMODE_UP,
	TIM_CountMode_Down = TIM_COUNTERMODE_DOWN,
	TIM_CountMode_Cen1 = TIM_COUNTERMODE_CENTERALIGNED1,
	TIM_CountMode_Cen2 = TIM_COUNTERMODE_CENTERALIGNED2,
	TIM_CountMode_Cen3 = TIM_COUNTERMODE_CENTERALIGNED3
}TIM_CountMode;*/

//enums for channel operate
typedef enum{
	CTIM_Channel_1 = 0,
	CTIM_CHannel_1N,
	CTIM_Channel_2,
	CTIM_Channel_2N,
	CTIM_Channel_3,
	CTIM_Channel_3N,
	CTIM_Channel_4,
	CTIM_Channel_4N,
}CTIM_CHx;

typedef struct{
	bool TIM_Update:1;
	bool TIM_CC1:1;
	bool TIM_CC2:1;
	bool TIM_CC3:1;
	bool TIM_CC4:1;
	bool TIM_COM:1;
	bool TIM_Trigger:1;
	bool TIM_Break:1;
}CTIM_Event;

typedef enum{
	CTIM_TRGO_Reset  = 0b000,
	CTIM_TRGO_Enable = 0b001,
	CTIM_TRGO_Update = 0b010,
	CTIM_TRGO_OC1    = 0b011,
	CTIM_TRGO_OC1Ref = 0b100,
	CTIM_TRGO_OC2Ref = 0b101,
	CTIM_TRGO_OC3Ref = 0b110,
	CTIM_TRGO_OC4Ref = 0b111,
}CTIM_TRGO;

//CCMR_comm
typedef enum{
	CTIM_Out   = 0b00,  //输出模式
	CTIM_In    = 0b01,  //输入模式
	CTIM_InX   = 0b10,  //输入模式，交换通道
	CTIM_InTRC = 0b11,  //输入模式，Slave模式
}CTIM_CCxS;

//CCMR_Out
typedef enum{
	CTIM_OCM_Timing         = 0b000,
	CTIM_OCM_Match_Active   = 0b001,
	CTIM_OCM_Match_Inactive = 0b010,
	CTIM_OCM_Match_Toggle   = 0b011,
	CTIM_OCM_Force_Inactive = 0b100,
	CTIM_OCM_Force_Active   = 0b101,
	CTIM_OCM_PWM_Mode1      = 0b110,
	CTIM_OCM_PWM_Mode2      = 0b111,
}CTIM_OCxM;

typedef struct{
	CTIM_CCxS       ccxs:2;
	FunctionalState fasten:1;
	FunctionalState preload:1;
	CTIM_OCxM       ocxm:3;
	FunctionalState clear:1;
}CTIM_CCMR_Out;
//CCMR_Out end

typedef enum{
	CTIM_ICPSC_1 = 0b00,
	CTIM_ICPSC_2 = 0b01,
	CTIM_ICPSC_4 = 0b10,
	CTIM_ICPSC_8 = 0b11,
}CTIM_ICPSC;

typedef enum{
	CTIM_ICF_No      = 0b0000,
	CTIM_ICF_CKINT_2 = 0b0001,
	CTIM_ICF_CKINT_4 = 0b0010,
	CTIM_ICF_CKINT_8 = 0b0011,
	CTIM_ICF_DTS2_6  = 0b0100,
	CTIM_ICF_DTS2_8  = 0b0101,
	CTIM_ICF_DTS4_6  = 0b0110,
	CTIM_ICF_DTS4_8  = 0b0111,
	CTIM_ICF_DTS8_6  = 0b1000,
	CTIM_ICF_DTS8_8  = 0b1001,
	CTIM_ICF_DTS16_5 = 0b1010,
	CTIM_ICF_DTS16_6 = 0b1011,
	CTIM_ICF_DTS16_8 = 0b1100,
	CTIM_ICF_DTS32_5 = 0b1101,
	CTIM_ICF_DTS32_6 = 0b1110,
	CTIM_ICF_DTS32_8 = 0b1111,
}CTIM_ICF;

typedef struct{
	CTIM_CCxS  ccxs:2;
	CTIM_ICPSC icpsc:2;
	CTIM_ICF   icf:4;
}CTIM_CCMR_In;

typedef union{
	CTIM_CCMR_Out Out;
	CTIM_CCMR_In  In;
}CTIM_CCMR;

/*
#define TIM_CH2IT(x) (x==TIM_Channel_1 ? TIM_IT_cc1:\
		             (x==TIM_Channel_2 ? TIM_IT_cc2:\
		             (x==TIM_Channel_3 ? TIM_IT_cc3:\
		             TIM_IT_cc4)))
*/

CTIM_TRGO TIM_CHx2TRGO(CTIM_CHx ch);

class C_TIM : public TIM_TypeDef{
public:
	//Trig and Clock
	void set_Trig(u32 trig, u32 polar, u32 prescale, u32 filter);
	void set_ExtClk(bool isExtern);
	//low level API get/set ClockDiv(CKD), PreScale(PSC), AutoLoad(ARR)
	inline u32 get_clockdiv_sft()		{return READ_BIT(this->CTLR1, TIM_CTLR1_CKD);}
	//inline u32 get_clockdiv_2n()		{return READ_BIT(this->CTLR1, TIM_CTLR1_CKD) >> CLZ(TIM_CTLR1_CKD) ;}
	inline u16 get_prescale()		{return READ_REG(this->PSC);}
	inline u16 get_autoload()		{return READ_REG(this->ATRLR);}
	inline void set_clockdiv_sft(u32 ckd)	{MODIFY_REG(this->CTLR1, TIM_CTLR1_CKD, ckd);}
	//inline void set_clockdiv_2n(u32 ckd_2n)	{MODIFY_REG(this->CTLR1, TIM_CTLR1_CKD, ckd_2n << CLZ(TIM_CTLR1_CKD));}
	inline void set_prescale(u32 psc)	{WRITE_REG(this->PSC, psc);}
	inline void set_autoload(u32 arr)	{WRITE_REG(this->ATRLR, arr);}
	//high level API get/set ClockDiv(CKD), PreScale(PSC), AutoLoad(ARR)
	TypeDiv get_div(TIM_ClockLevel level);
	Type_Hz get_Hz(TIM_ClockLevel level);
	Type_ns get_ns(TIM_ClockLevel level);
	TypeDiv set_tdiv(TypeDiv div);
	void set_Hz(Type_Hz Hz);
	void set_ns(Type_ns ns);
	void set_us(u32 us);
	//others
	void set_TRGO(CTIM_TRGO trgo, bool ms_enable);
	void set_CountEnable(bool isEnable);
	//inline void set_CountMode(TIM_CountMode mode) {MODIFY_REG(this->CTRL1, TIM_CR1_DIR|TIM_CR1_CMS, mode);};
	/*inline void reset_count(){
		uint32_t cnt = __HAL_TIM_IS_TIM_COUNTING_DOWN(htim)?__HAL_TIM_GET_AUTORELOAD(htim):0;
		__HAL_TIM_SET_COUNTER(htim, cnt);
	}*/
	u32 maxcount();

	//TODO: set comp ns
	//TODO: add DMA support
	//operate single channel
	u16  get_comp(CTIM_CHx Channel);
	u16  get_duty(CTIM_CHx Channel);
	void set_comp(CTIM_CHx Channel, u16 comp);
	void set_duty(CTIM_CHx Channel, u16 duty);
	//operate 4 channels
	void set_comp4(u32 comp1, u32 comp2, u32 comp3, u32 comp4);
	void set_duty4(float duty1, float duty2, float duty3, float duty4);
	//OnePluse Mode
	void pluse_clk(CTIM_CHx Channel, u32 delay_clk, u32 pluse_clk, bool blocking);
	void pluse_ns(CTIM_CHx Channel, u32 delay_ns, u32 pluse_ns, bool blocking);
	//others
	void CCxChannelCmd(CTIM_CHx Channel, FunctionalState ChannelState);
	void CCxChannelPolar(CTIM_CHx Channel, PinState ActiveLevel);

	X_State Set_CCMR(CTIM_CHx Channel, CTIM_CCMR *ccmr);

	void PWM_Init();
	void PWM_Start(CTIM_CHx Channel);
	void Force_Active(CTIM_CHx Channel);
	void Force_Inactive(CTIM_CHx Channel);

	inline uint32_t ReadCapturedValue(CTIM_CHx Channel);
	//old C_TIMEx
	//void set_callback(CTIM_IT IT, void (*func)(void*), void* param);
	//void clear_callback(CTIM_IT IT);
	//void from_ISR();
};



//single channel class
class TIM_CH{
private:
	C_TIM *htim;
	CTIM_CHx Channel;
	bool allowCNT;
public:
	TIM_CH(C_TIM *htim, CTIM_CHx Channel, bool allowCNT);
	inline u16  get_comp()    {return htim->get_comp(Channel);}
	inline u16  get_duty()    {return htim->get_duty(Channel);}
	inline void set_comp(u16 comp)   {htim->set_comp(Channel, comp);}
	inline void set_duty(u16 duty) {htim->set_duty(Channel, duty);}
	//OnePluse Mode
	void pluse_ns(u32 delay_ns, u32 pluse_ns, bool blocking);
	void pluse_clk(u32 delay_clk, u32 pluse_clk, bool blocking);
	//others
	//void set_OCMode(TIM_OCMode mode);
	//void CCxChannelCmd(TIM_CCxE ChannelState);

	Type_Hz get_Hz(TIM_ClockLevel level){return htim->get_Hz(level);}
	Type_ns get_ns(TIM_ClockLevel level){return htim->get_ns(level);}
	void set_Hz(Type_Hz Hz)             {htim->set_Hz(Hz);}
	void set_ns(Type_ns ns)             {htim->set_ns(ns);}

	//void EnableIT()           {htim->EnableIT(TIM_CH2IT(Channel));}
	//void DisableIT()          {htim->DisableIT(TIM_CH2IT(Channel));}

	//void start()	{CCxChannelCmd(ENABLE);}
	//void stop()	{CCxChannelCmd(DISABLE);}
};

extern "C"{
	void C_TIMEx_ISR_func(C_TIM* ctim);
}

#endif /* INC_TIMER_HPP_ */
