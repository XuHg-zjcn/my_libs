#include "f1_gpio.hpp"
#include "bit_band.h"


Pin8b::Pin8b(uint32_t port, uint32_t pin){
	PORTx = port;
	PINx = pin;
}

Pin8b::Pin8b(GPIO_TypeDef *GPIOx, uint32_t pin2N){
	PORTx = (((uint32_t)GPIOx) - GPIOA_BASE)/0x400;
	PINx = __builtin_ctz(pin2N);
}


GPIO_TypeDef* Pin8b::GPIOx()
{
	return (GPIO_TypeDef *)(GPIOA_BASE + 0x400*PORTx);
}

uint32_t Pin8b::Pin2N()
{
	return 1<<PINx;
}

uint32_t* Pin8b::ODR_bitband()
{
	return BIT_PTR(&(GPIOx()->ODR), PINx);
}

uint32_t* Pin8b::IDR_bitband()
{
	return BIT_PTR(&(GPIOx()->IDR), PINx);
}

void Pin8b::loadCfg(PinCfg cfg)
{
    if(PORTx >= TOTAL_PORTS){
        return;
    }
    GPIO_TypeDef *gpiox = GPIOx();
    uint32_t pin2N = Pin2N();
    
    //config CNF and MODE on CRL/CRH
    __IO uint32_t *CRx = (PINx < 8) ? &gpiox->CRL : &gpiox->CRH;
    //if(((uint32_t)cfg) & CNF_MODE_Msk != CNF_MODE_KEEP){ //test faild
        uint32_t reg_offset = (PINx & 0x07) << 2u;
        MODIFY_REG((*CRx), (CNF_MODE_Msk << reg_offset), ((cfg&CNF_MODE_Msk) << reg_offset));
    //}//else{keep old config};
    
    //config ODR
    if(cfg & ODR_Msk){
        gpiox->BSRR = pin2N;
    }else{
        gpiox->BSRR = pin2N << 16;
    }
}

void Pin8b::lockCfg()
{
    HAL_GPIO_LockPin(GPIOx(), Pin2N());
}

void Pin8b::setEXTI(EnumEXTI exti)
{
    uint32_t pin2N = Pin2N();
    if(exti & (EXTI_RISE_Msk | EXTI_FALL_Msk)){
        //config AFIO->EXTICR
        uint32_t temp = AFIO->EXTICR[PINx >> 2u];
        CLEAR_BIT(temp, (0x0Fu) << (4u * (PINx & 0x03u)));
        SET_BIT(temp, PORTx << (4u * (PINx & 0x03u)));
        AFIO->EXTICR[PINx >> 2] = temp;
        //config rising/falling edge
        if(exti & EXTI_RISE_Msk){
            SET_BIT(EXTI->RTSR, pin2N);
        }if(exti & EXTI_FALL_Msk){
            SET_BIT(EXTI->FTSR, pin2N);
        }
        //config Interrupt/Event
        if(exti & EXTI_EVT_Msk){
            CLEAR_BIT(EXTI->IMR, pin2N);
            SET_BIT(EXTI->EMR, pin2N);
        }else{
            CLEAR_BIT(EXTI->EMR, pin2N);
            SET_BIT(EXTI->IMR, pin2N);
        }
    }
}
