#include "c_tim.hpp"
#ifdef __C_TIM_HPP__

void C_TIM::CCxChannelCmd(CTIM_CHx Channel, FunctionalState ChannelState)
{
	if(ChannelState != DISABLE){
		SET_BIT(this->CCER, 1U<<(Channel*2));
	}else{
		CLEAR_BIT(this->CCER, 1U<<(Channel*2));
	}
}

void C_TIM::CCxChannelPolar(CTIM_CHx Channel, PinState ActiveLevel)
{
	if(ActiveLevel != Pin_Reset){
		SET_BIT(this->CCER, 1U<<(Channel*2+1));
	}else{
		CLEAR_BIT(this->CCER, 1U<<(Channel*2+1));
	}
}

void C_TIM::set_comp(CTIM_CHx channel, u16 comp)
{
	*((&(this->CH1CVR))+channel) = comp;
}

void C_TIM::set_duty(CTIM_CHx channel, u16 duty)
{
	set_comp(channel, (u16)(((u32)get_autoload()*duty)>>16));
}

X_State C_TIM::Set_CCMR(CTIM_CHx channel, CTIM_CCMR *ccmr)
{
	switch(channel){
		case CTIM_Channel_1:
			MODIFY_REG(this->CHCTLR1, 0x00ff, (*(u16*)ccmr));
			break;
		case CTIM_Channel_2:
			MODIFY_REG(this->CHCTLR1, 0xff00, (*(u16*)ccmr)<<8);
			break;
		case CTIM_Channel_3:
			MODIFY_REG(this->CHCTLR2, 0x00ff, (*(u16*)ccmr));
			break;
		case CTIM_Channel_4:
			MODIFY_REG(this->CHCTLR2, 0xff00, (*(u16*)ccmr)<<8);
			break;
		default:
			return X_InvaildParam;
	}
	return X_OK;
}

void C_TIM::PWM_Init()
{
	this->CTLR1 = 0x0000;
	this->CTLR2 = 0x0000;
	this->SMCFGR = 0x0000;    //关闭从模式
	this->DMAINTENR = 0x0000; //关闭所有中断和DMA触发
}

void C_TIM::PWM_Start(CTIM_CHx Channel)
{
	CTIM_CCMR ccmr;
	ccmr.Out = {CTIM_Out, DISABLE, DISABLE, CTIM_OCM_PWM_Mode2, DISABLE};
	Set_CCMR(Channel, &ccmr);
	SET_BIT(this->CTLR1, TIM_CEN);
	CCxChannelCmd(Channel, ENABLE);
}

#endif /* __C_TIM_HPP__ */
