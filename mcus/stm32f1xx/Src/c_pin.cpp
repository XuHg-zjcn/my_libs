#include "c_pin.hpp"
#include "bit_band.h"


C_Pin::C_Pin(uint32_t port, uint32_t pin){
	PORTx = port;
	PINx = pin;
}

C_Pin::C_Pin(std::initializer_list<uint32_t> lst)
{
	PORTx = *lst.begin();
	PINx = *(lst.begin()+1);
}

C_Pin::C_Pin(GPIO_TypeDef *GPIOx, uint32_t pin2N){
	PORTx = (((uint32_t)GPIOx) - GPIOA_BASE)/0x400;
	PINx = __builtin_ctz(pin2N);
}

/*
 * create C_Pin object from bitband address of IDR or ODR.
 * untested.
 */
C_Pin::C_Pin(u32 *bitband)
{   //TODO: check param
	PORTx = ((BITBAND_SRCADDR(bitband)&0xfffffc00) - GPIOA_BASE)/0x400;
	PINx = (((uint32_t)bitband&0x40)/4);
}


GPIO_TypeDef* C_Pin::GPIOx()
{
	return (GPIO_TypeDef *)(GPIOA_BASE + 0x400*PORTx);
}

uint32_t C_Pin::Pin2N()
{
	return 1<<PINx;
}

uint32_t* C_Pin::ODR_bitband()
{
	return BIT_PTR(&(GPIOx()->ODR), PINx);
}

uint32_t* C_Pin::IDR_bitband()
{
	return BIT_PTR(&(GPIOx()->IDR), PINx);
}

void C_Pin::write_pin(PinState x)
{
	HAL_GPIO_WritePin(GPIOx(), Pin2N(), (GPIO_PinState)x);
}

PinState C_Pin::read_pin()
{
	return (PinState)HAL_GPIO_ReadPin(GPIOx(), Pin2N());
}

void C_Pin::toggle_pin()
{
	HAL_GPIO_TogglePin(GPIOx(), Pin2N());
}

#ifndef USE_VIRTUAL
//blocking until read_pin() == state
void C_Pin::wait_pin(PinState state)
{
	while(read_pin() xor state);
}

u32 C_Pin::wait_timeout(PinState state, u32 timeout)
{
	while((read_pin() xor state) and timeout){
		timeout--;
	}
	return timeout;
}

//阻塞式测量
u32 C_Pin::wait_count(PinState state, u32 m, u32 M)
{
	u32 n=0;
	state = !state;
	while((read_pin() xor state) and n<M or n<m){
		n++;
	}
	return n;
}
#endif

PinXSpd C_Pin::MHz2Spd(u8 MHz)
{
	const PinXSpd xSpd[3] = {Out_2MHz, Out_10MHz, Out_50MHz};
	const uint8_t xMHz[3] = {2, 10, 50};
	int i=0;
	while(xMHz[i]<MHz && i<2){
		i++;
	}
	return xSpd[i];
}

void C_Pin::loadCfg(PinCfg cfg, u8 MHz)
{
	const PinXCfg xcfgs[] = {
		GPIO_GP_PP0,
		GPIO_GP_PP1,
		GPIO_GP_OD0,
		GPIO_GP_OD1,
		GPIO_In_Up,
		GPIO_In_Float,
		GPIO_In_Down};
	PinXCfg xcfg = xcfgs[cfg];
	if(PIN_IS_OUT(cfg)){
		xcfg &= MHz2Spd(MHz);
	}
	loadXCfg(xcfg);
}

void C_Pin::loadCfg(PinCfg cfg)
{
	loadCfg(cfg, 255);
}

void C_Pin::loadXCfg(PinXCfg cfg)
{
    if(PORTx >= TOTAL_GPIO_PORTS){
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

void C_Pin::lockCfg()
{
    HAL_GPIO_LockPin(GPIOx(), Pin2N());
}

void C_Pin::setEXTI(EnumEXTI exti)
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
