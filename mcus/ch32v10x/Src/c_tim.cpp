#include "c_tim.hpp"
#ifdef __C_TIM_HPP__

#include "ops.hpp"

extern uint32_t SystemCoreClock;

TypeDiv C_TIM::get_div(TIM_ClockLevel level)
{
  TypeDiv div=1;
  //without break, exec code after case.
  switch(level){
  case TIM_ClkLv_AutoLoad:
    div *= (ATRLR + 1);
  case TIM_ClkLv_PSC16b:
    div *= (PSC + 1);
  case TIM_ClkLv_CKD:
    div <<= CTLR1.CKD;
  default:
    break;
  }
  return div;
}

Type_Hz C_TIM::get_Hz(TIM_ClockLevel level)
{
  return SystemCoreClock/get_div(level);
}

Type_ns C_TIM::get_ns(TIM_ClockLevel level)
{
  return (get_div(level)*1000000000)/SystemCoreClock;
}

void C_TIM::set_div(TypeDiv div)
{
  CTIM_CKD ckd;
  u32 x, y, xy;
  u32 err = 0xffffffff;
  u32 bestx;
  if(div==0 || div > (4ULL<<32)){
    return;
  }else if(div > (2ULL<<32)){
    ckd = CTIM_CKD_4;
    xy = div/4;
  }else if(div > (1ULL<<32)){
    ckd = CTIM_CKD_2;
    xy = div/2;
  }else if(div == (1ULL<<32)){
    ckd = CTIM_CKD_No;
    xy = div;
  }
  if(xy == 0){
    x = 65536;
    y = 65536;
    goto calc;
  }
  if(xy <= 65536){
    x = 1;
    y = xy;
    goto calc;
  }
  x = CEIL_DIV(xy, 65536);  //x的搜索启始值，y不能超过65536
  for(;x<=65536;x++){
    y = ROUND_DIV(xy, x);
    if(abs(xy - x*y) < err){
      err = abs(xy - x*y);
      bestx = x;
      if(err == 0){
	goto calc;
      }
    }
  }
  y = ROUND_DIV(xy, bestx);
 calc:
  CTLR1.CKD = ckd;
  PSC = x - 1;
  ATRLR = y - 1;
}

void C_TIM::set_Hz(Type_Hz Hz)
{
  set_div((TypeDiv)(SystemCoreClock/Hz));
}

void C_TIM::set_ns(Type_ns ns)
{
  set_div((TypeDiv)((SystemCoreClock*ns)/1000000000));
}

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
  set_comp(channel, (u16)(((u32)(ATRLR)*duty)>>16));
}

X_State C_TIM::Set_CCMR(CTIM_CHx channel, CTIM_CCMR *ccmr)
{
  switch(channel){
  case CTIM_Channel_1:
    this->CHCTLR1.L = *ccmr;
    break;
  case CTIM_Channel_2:
    this->CHCTLR1.H = *ccmr;
    break;
  case CTIM_Channel_3:
    this->CHCTLR2.L = *ccmr;
    break;
  case CTIM_Channel_4:
    this->CHCTLR2.H = *ccmr;
    break;
  default:
    return X_InvaildParam;
  }
  return X_OK;
}

void C_TIM::PWM_Init()
{
  CAST(u16, this->CTLR1) = 0x0000;
  CAST(u16, this->CTLR2) = 0x0000;
  CAST(u16, this->SMCFGR) = 0x0000;    //关闭从模式
  CAST(u16, this->DMAINTENR) = 0x0000; //关闭所有中断和DMA触发
}

void C_TIM::PWM_Start(CTIM_CHx Channel)
{
  CTIM_CCMR ccmr;
  ccmr.Out = {CTIM_Out, DISABLE, DISABLE, CTIM_OCM_PWM_Mode2, DISABLE};
  Set_CCMR(Channel, &ccmr);
  this->CTLR1.CEN = true;
  CCxChannelCmd(Channel, ENABLE);
}

#endif /* __C_TIM_HPP__ */
