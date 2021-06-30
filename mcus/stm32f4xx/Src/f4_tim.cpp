#include "f4_tim.hpp"

u32 F4_TIM::maxcount()
{
	TIM_TypeDef *Instance = this->Instance;
	if(Instance == TIM2 || Instance == TIM5){
		return 0xffffffff;
	}else{
		return 0xffff;
	}
}
