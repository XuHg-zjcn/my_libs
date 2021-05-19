#include "f1_gpio.hpp"

void loadPinCfg(Pin_8b pin, UnionPinCfg cfg)
{
    if(pin.PORTx >= TOTAL_PORTS){
        return;
    }
    GPIO_TypeDef *GPIOx = p8b_GPIOx(pin);
    uint32_t pin2N = p8b_Pin2N(pin);
    
    //config CNF and MODE on CRL/CRH
    __IO uint32_t *CRx = (pin.PINx < 8) ? &GPIOx->CRL : &GPIOx->CRH;
    if(cfg.e & CNF_MODE_Msk != CNF_MODE_KEEP){
        uint32_t reg_offset = (pin.PINx & 0x07) << 2u;
        MODIFY_REG((*CRx), (CNF_MODE_Msk << reg_offset), (cfg.e & CNF_MODE_Msk << reg_offset));
    }//else{keep old config};
    
    //config ODR
    if(cfg.s.odr){
        GPIOx->BSRR = pin2N;
    }else{
        GPIOx->BSRR = pin2N << 16;
    }
    
    //config EXTI
    if(cfg.s.exti & (EXTI_RISE_Msk | EXTI_FALL_Msk)){
        //config AFIO->EXTICR
        uint32_t temp = AFIO->EXTICR[pin.PINx >> 2u];
        CLEAR_BIT(temp, (0x0Fu) << (4u * (pin.PINx & 0x03u)));
        SET_BIT(temp, pin.PORTx << (4u * (pin.PINx & 0x03u)));
        AFIO->EXTICR[pin.PINx >> 2] = temp;
        //config rising/falling edge
        if(cfg.s.exti & EXTI_RISE_Msk){
            SET_BIT(EXTI->RTSR, pin2N);
        }if(cfg.s.exti & EXTI_FALL_Msk){
            SET_BIT(EXTI->FTSR, pin2N);
        }
        //config Interrupt/Event
        if(cfg.s.exti & EXTI_EVT_Msk){
            CLEAR_BIT(EXTI->IMR, pin2N);
            SET_BIT(EXTI->EMR, pin2N);
        }else{
            CLEAR_BIT(EXTI->EMR, pin2N);
            SET_BIT(EXTI->IMR, pin2N);
        }
    }
}

void LockPin(Pin_8b pin)
{
    HAL_GPIO_LockPin(p8b_GPIOx(pin), p8b_Pin2N(pin));
}
