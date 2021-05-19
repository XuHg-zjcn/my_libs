#include "f1_gpio.hpp"

void loadPinCfg(Pin_8b pin, PinCfg cfg)
{
    if(pin.PORTx >= TOTAL_PORTS){
        return;
    }
    GPIO_TypeDef *GPIOx = p8b_GPIOx(pin);
    uint32_t pin2N = p8b_Pin2N(pin);
    
    //config CNF and MODE on CRL/CRH
    __IO uint32_t *CRx = (pin.PINx < 8) ? &GPIOx->CRL : &GPIOx->CRH;
    //if(((uint32_t)cfg) & CNF_MODE_Msk != CNF_MODE_KEEP){ //test faild
        uint32_t reg_offset = (pin.PINx & 0x07) << 2u;
        MODIFY_REG((*CRx), (CNF_MODE_Msk << reg_offset), ((cfg&CNF_MODE_Msk) << reg_offset));
    //}//else{keep old config};
    
    //config ODR
    if(cfg & ODR_Msk){
        GPIOx->BSRR = pin2N;
    }else{
        GPIOx->BSRR = pin2N << 16;
    }
}

void LockPin(Pin_8b pin)
{
    HAL_GPIO_LockPin(p8b_GPIOx(pin), p8b_Pin2N(pin));
}
