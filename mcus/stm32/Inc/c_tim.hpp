/*
 * timer.h
 *
 *  Created on: Apr 17, 2021
 *      Author: xrj
 */

#if !defined(INC_STM32_TIM_HPP_) && defined(HAL_TIM_MODULE_ENABLED)
#define INC_STM32_TIM_HPP_

#include "myints.h"
#include "mylibs_config.hpp"

#define N_IT     8           //number interrupts of timer
#define CompKeep 0xffffffff  //flag for `set_comp4()` keep old compare value.
#define DutyKeep -1          //flag for `set_duty4()` keep old compare value.

//TODO: use #include<ratio>
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

typedef enum{
	TIM_IT_update = TIM_IT_UPDATE,
	TIM_IT_cc1 = TIM_IT_CC1,
	TIM_IT_cc2 = TIM_IT_CC2,
	TIM_IT_cc3 = TIM_IT_CC3,
	TIM_IT_cc4 = TIM_IT_CC4,
	TIM_IT_com = TIM_IT_COM,
	TIM_IT_trigger = TIM_IT_TRIGGER,
	TIM_IT_break = TIM_IT_BREAK,
	TIM_IT_all = 0xfff
}TIM_IT;

typedef enum{
	TIM_TRGO_reset = TIM_TRGO_RESET,
	TIM_TGRO_enable = TIM_TRGO_ENABLE,
	TIM_TGRO_update = TIM_TRGO_UPDATE,
	TIM_TRGO_oc1 = TIM_TRGO_OC1,
	TIM_TRGO_oc1ref = TIM_TRGO_OC1REF,
	TIM_TRGO_oc2ref = TIM_TRGO_OC2REF,
	TIM_TRGO_oc3ref = TIM_TRGO_OC3REF,
	TIM_TRGO_oc4ref = TIM_TRGO_OC4REF,
}TIM_TRGO;

#define TIM_CH2IT(x) (x==TIM_Channel_1 ? TIM_IT_cc1:\
		             (x==TIM_Channel_2 ? TIM_IT_cc2:\
		             (x==TIM_Channel_3 ? TIM_IT_cc3:\
		             TIM_IT_cc4)))

TIM_TRGO TIM_CHx2TRGO(TIM_CHx ch);

class C_TIM{
private:
	void* params[N_IT];
	void (*callbacks[N_IT])(void*);
public:
	TIM_HandleTypeDef *htim;
	C_TIM(TIM_HandleTypeDef *htim);
	//Trig and Clock
	void set_Trig(u32 trig, u32 polar, u32 prescale, u32 filter);
	void set_ExtClk(bool isExtern);
	//low level API get/set ClockDiv(CKD), PreScale(PSC), AutoLoad(ARR)
	inline u32 get_clockdiv_sft()			{return __HAL_TIM_GET_CLOCKDIVISION(htim);}
	inline u32 get_clockdiv_2n()			{return __HAL_TIM_GET_CLOCKDIVISION(htim) >> TIM_CR1_CKD_Pos;}
	inline u32 get_prescale()				{return READ_REG(htim->Instance->PSC);}
	inline u32 get_autoload()				{return __HAL_TIM_GET_AUTORELOAD(htim);}
	inline void set_clockdiv_sft(u32 ckd)	{__HAL_TIM_SET_CLOCKDIVISION(htim, ckd);}
	inline void set_clockdiv_2n(u32 ckd_2n)	{__HAL_TIM_SET_CLOCKDIVISION(htim, ckd_2n << TIM_CR1_CKD_Pos);}
	inline void set_prescale(u32 psc)		{__HAL_TIM_SET_PRESCALER(htim, psc);}
	inline void set_autoload(u32 arr)		{__HAL_TIM_SET_AUTORELOAD(htim, arr);}
	//high level API get/set ClockDiv(CKD), PreScale(PSC), AutoLoad(ARR)
	TypeDiv get_div(TIM_ClockLevel level);
	Type_Hz get_Hz(TIM_ClockLevel level);
	Type_ns get_ns(TIM_ClockLevel level);
	TypeDiv set_tdiv(TypeDiv div);
	void set_Hz(Type_Hz Hz);
	void set_ns(Type_ns ns);
	void set_us(u32 us);
	//others
	void set_TGRO(TIM_TRGO trgo, bool ms_enable);
	void set_CountEnable(bool isEnable);
	inline void set_CountMode(TIM_CountMode mode) {MODIFY_REG(htim->Instance->CR1, TIM_CR1_DIR|TIM_CR1_CMS, mode);};
	inline void reset_count(){
		uint32_t cnt = __HAL_TIM_IS_TIM_COUNTING_DOWN(htim)?__HAL_TIM_GET_AUTORELOAD(htim):0;
		__HAL_TIM_SET_COUNTER(htim, cnt);
	}
	u32 maxcount();

	//TODO: set comp ns
	//TODO: add DMA support
	//operate single channel
	inline u32   get_comp(TIM_CHx Channel)    {return __HAL_TIM_GET_COMPARE(htim, Channel);}
	inline float get_duty(TIM_CHx Channel)    {return (float)__HAL_TIM_GET_COMPARE(htim, Channel)/__HAL_TIM_GET_AUTORELOAD(htim);}
	inline void set_comp(TIM_CHx Channel, u32 comp)   {__HAL_TIM_SET_COMPARE(htim, Channel, comp);}
	inline void set_duty(TIM_CHx Channel, float duty) {__HAL_TIM_SET_COMPARE(htim, Channel, duty*__HAL_TIM_GET_AUTORELOAD(htim));}
	//operate 4 channels
	void set_comp4(u32 comp1, u32 comp2, u32 comp3, u32 comp4);
	void set_duty4(float duty1, float duty2, float duty3, float duty4);
	//OnePluse Mode
	void pluse_clk(TIM_CHx Channel, u32 delay_clk, u32 pluse_clk, bool blocking);
	void pluse_ns(TIM_CHx Channel, u32 delay_ns, u32 pluse_ns, bool blocking);
	//others
	void set_OCMode(TIM_CHx Channel, TIM_OCMode mode);
	void CCxChannelCmd(TIM_CHx Channel, TIM_CCxE ChannelState);

	void EnableIT(TIM_IT IT)            {__HAL_TIM_ENABLE_IT(htim, IT);}
	void DisableIT(TIM_IT IT)           {__HAL_TIM_DISABLE_IT(htim, IT);}
	void ClearITFlag(TIM_IT IT)         {__HAL_TIM_CLEAR_IT(htim, IT);}

	/* Time Base functions ********************************************************/
	inline HAL_StatusTypeDef Base_Init()      {return HAL_TIM_Base_Init(htim);}
	inline HAL_StatusTypeDef Base_Deinit()    {return HAL_TIM_Base_DeInit(htim);}
	inline void Base_MspInit()                {HAL_TIM_Base_MspInit(htim);}
	inline void Base_MspDeInit()              {HAL_TIM_Base_MspDeInit(htim);}
	inline HAL_StatusTypeDef Base_Start()     {return HAL_TIM_Base_Start(htim);}
	inline HAL_StatusTypeDef Base_Stop()      {return HAL_TIM_Base_Stop(htim);}
	inline HAL_StatusTypeDef Base_Start_IT()  {return HAL_TIM_Base_Start_IT(htim);}
	inline HAL_StatusTypeDef Base_Stop_IT()   {return HAL_TIM_Base_Stop_IT(htim);}
	inline HAL_StatusTypeDef Base_Start_DMA(uint32_t *pData, uint16_t Length)
		                     {return HAL_TIM_Base_Start_DMA(htim, pData, Length);}
	inline HAL_StatusTypeDef Base_Stop_DMA()  {return HAL_TIM_Base_Stop_DMA(htim);}

	/* Timer Output Compare functions *********************************************/
	inline HAL_StatusTypeDef OC_Init()      {return HAL_TIM_OC_Init(htim);}
	inline HAL_StatusTypeDef OC_Deinit()    {return HAL_TIM_OC_DeInit(htim);}
	inline void OC_MspInit()                {HAL_TIM_OC_MspInit(htim);}
	inline void OC_MspDeInit()              {HAL_TIM_OC_MspDeInit(htim);}
	inline HAL_StatusTypeDef OC_Start(TIM_CHx Channel)     {return HAL_TIM_OC_Start(htim, Channel);}
	inline HAL_StatusTypeDef OC_Stop(TIM_CHx Channel)      {return HAL_TIM_OC_Stop(htim, Channel);}
	inline HAL_StatusTypeDef OC_Start_IT(TIM_CHx Channel)  {return HAL_TIM_OC_Start_IT(htim, Channel);}
	inline HAL_StatusTypeDef OC_Stop_IT(TIM_CHx Channel)   {return HAL_TIM_OC_Stop_IT(htim, Channel);}
	inline HAL_StatusTypeDef OC_Start_DMA(TIM_CHx Channel, uint32_t *pData, uint16_t Length)
		                     {return HAL_TIM_OC_Start_DMA(htim, Channel, pData, Length);}
	inline HAL_StatusTypeDef OC_Stop_DMA(TIM_CHx Channel)  {return HAL_TIM_OC_Stop_DMA(htim, Channel);}

	/* Timer PWM functions ********************************************************/
	inline HAL_StatusTypeDef PWM_Init()      {return HAL_TIM_PWM_Init(htim);}
	inline HAL_StatusTypeDef PWM_Deinit()    {return HAL_TIM_PWM_DeInit(htim);}
	inline void PWM_MspInit()                {HAL_TIM_PWM_MspInit(htim);}
	inline void PWM_MspDeInit()              {HAL_TIM_PWM_MspDeInit(htim);}
	inline HAL_StatusTypeDef PWM_Start(TIM_CHx Channel)     {return HAL_TIM_PWM_Start(htim, Channel);}
	inline HAL_StatusTypeDef PWM_Stop(TIM_CHx Channel)      {return HAL_TIM_PWM_Stop(htim, Channel);}
	inline HAL_StatusTypeDef PWM_Start_IT(TIM_CHx Channel)  {return HAL_TIM_PWM_Start_IT(htim, Channel);}
	inline HAL_StatusTypeDef PWM_Stop_IT(TIM_CHx Channel)   {return HAL_TIM_PWM_Stop_IT(htim, Channel);}
	inline HAL_StatusTypeDef PWM_Start_DMA(TIM_CHx Channel, uint32_t *pData, uint16_t Length)
		                     {return HAL_TIM_PWM_Start_DMA(htim, Channel, pData, Length);}
	inline HAL_StatusTypeDef PWM_Stop_DMA(TIM_CHx Channel)  {return HAL_TIM_PWM_Stop_DMA(htim, Channel);}

	/* Timer Input Capture functions **********************************************/
	inline HAL_StatusTypeDef IC_Init()      {return HAL_TIM_IC_Init(htim);}
	inline HAL_StatusTypeDef IC_Deinit()    {return HAL_TIM_IC_DeInit(htim);}
	inline void IC_MspInit()                {HAL_TIM_IC_MspInit(htim);}
	inline void IC_MspDeInit()              {HAL_TIM_IC_MspDeInit(htim);}
	inline HAL_StatusTypeDef IC_Start(TIM_CHx Channel)     {return HAL_TIM_IC_Start(htim, Channel);}
	inline HAL_StatusTypeDef IC_Stop(TIM_CHx Channel)      {return HAL_TIM_IC_Stop(htim, Channel);}
	inline HAL_StatusTypeDef IC_Start_IT(TIM_CHx Channel)  {return HAL_TIM_IC_Start_IT(htim, Channel);}
	inline HAL_StatusTypeDef IC_Stop_IT(TIM_CHx Channel)   {return HAL_TIM_IC_Stop_IT(htim, Channel);}
	inline HAL_StatusTypeDef IC_Start_DMA(TIM_CHx Channel, uint32_t *pData, uint16_t Length)
		                     {return HAL_TIM_IC_Start_DMA(htim, Channel, pData, Length);}
	inline HAL_StatusTypeDef IC_Stop_DMA(TIM_CHx Channel)  {return HAL_TIM_IC_Stop_DMA(htim, Channel);}

	/* Timer One Pulse functions **************************************************/
	inline HAL_StatusTypeDef OnePulse_Init(u32 mode)            {return HAL_TIM_OnePulse_Init(htim, mode);}
	inline HAL_StatusTypeDef OnePulse_Deinit()                  {return HAL_TIM_OnePulse_DeInit(htim);}
	inline void OnePulse_MspInit()                              {HAL_TIM_OnePulse_MspInit(htim);}
	inline void OnePulse_MspDeInit()                            {HAL_TIM_OnePulse_MspDeInit(htim);}
	inline HAL_StatusTypeDef OnePulse_Start(TIM_CHx Channel)    {return HAL_TIM_OnePulse_Start(htim, Channel);}
	inline HAL_StatusTypeDef OnePulse_Stop(TIM_CHx Channel)     {return HAL_TIM_OnePulse_Stop(htim, Channel);}
	inline HAL_StatusTypeDef OnePulse_Start_IT(TIM_CHx Channel) {return HAL_TIM_OnePulse_Start_IT(htim, Channel);}
	inline HAL_StatusTypeDef OnePulse_Stop_IT(TIM_CHx Channel)  {return HAL_TIM_OnePulse_Stop_IT(htim, Channel);}

	/* Timer Encoder functions ****************************************************/
	inline HAL_StatusTypeDef Encoder_Init(TIM_Encoder_InitTypeDef *sConfig)
							{return HAL_TIM_Encoder_Init(htim, sConfig);}
	inline HAL_StatusTypeDef Encoder_Deinit()    {return HAL_TIM_Encoder_DeInit(htim);}
	inline void Encoder_MspInit()                {HAL_TIM_Encoder_MspInit(htim);}
	inline void Encoder_MspDeInit()              {HAL_TIM_Encoder_MspDeInit(htim);}
	inline HAL_StatusTypeDef Encoder_Start(TIM_CHx Channel)     {return HAL_TIM_Encoder_Start(htim, Channel);}
	inline HAL_StatusTypeDef Encoder_Stop(TIM_CHx Channel)      {return HAL_TIM_Encoder_Stop(htim, Channel);}
	inline HAL_StatusTypeDef Encoder_Start_IT(TIM_CHx Channel)  {return HAL_TIM_Encoder_Start_IT(htim, Channel);}
	inline HAL_StatusTypeDef Encoder_Stop_IT(TIM_CHx Channel)   {return HAL_TIM_Encoder_Stop_IT(htim, Channel);}
	inline HAL_StatusTypeDef Encoder_Start_DMA(TIM_CHx Channel, u32 *pData1, u32 *pData2, u16 Length)
		                     {return HAL_TIM_Encoder_Start_DMA(htim, Channel, pData1, pData2, Length);}
	inline HAL_StatusTypeDef Encoder_Stop_DMA(TIM_CHx Channel)  {return HAL_TIM_Encoder_Stop_DMA(htim, Channel);}

	inline uint32_t ReadCapturedValue(TIM_CHx Channel)          {return HAL_TIM_ReadCapturedValue(htim, Channel);}
	//old C_TIMEx
	void set_callback(TIM_IT IT, void (*func)(void*), void* param);
	void clear_callback(TIM_IT IT);
	void from_ISR();
};



//single channel class
class TIM_CH{
private:
	C_TIM *htim;
	TIM_CHx Channel;
	bool allowCNT;
public:
	TIM_CH(C_TIM *htim, TIM_CHx Channel, bool allowCNT);
	inline u32   get_comp()    {return htim->get_comp(Channel);}
	inline float get_duty()    {return htim->get_duty(Channel);}
	inline void set_comp(u32 comp)   {htim->set_comp(Channel, comp);}
	inline void set_duty(float duty) {htim->set_duty(Channel, duty);}
	//OnePluse Mode
	void pluse_ns(u32 delay_ns, u32 pluse_ns, bool blocking);
	void pluse_clk(u32 delay_clk, u32 pluse_clk, bool blocking);
	//others
	void set_OCMode(TIM_OCMode mode);
	void CCxChannelCmd(TIM_CCxE ChannelState);

	Type_Hz get_Hz(TIM_ClockLevel level){return htim->get_Hz(level);}
	Type_ns get_ns(TIM_ClockLevel level){return htim->get_ns(level);}
	void set_Hz(Type_Hz Hz)             {htim->set_Hz(Hz);}
	void set_ns(Type_ns ns)             {htim->set_ns(ns);}

	void EnableIT()           {htim->EnableIT(TIM_CH2IT(Channel));}
	void DisableIT()          {htim->DisableIT(TIM_CH2IT(Channel));}

	void start()	{CCxChannelCmd(TIM_CCx_Enable);}
	void stop()		{CCxChannelCmd(TIM_CCx_Disable);}
};

extern "C"{
	void C_TIMEx_ISR_func(C_TIM* ctim);
}

#endif /* INC_TIMER_HPP_ */
