#include "c_pin.hpp"
#ifdef __C_PIN_HPP__

#include "ops.hpp"


C_Pin::C_Pin(uint32_t port, uint32_t pin):
PORTx(port),PINx(pin)
{};

C_Pin::C_Pin(std::initializer_list<uint32_t> lst):
PORTx(*lst.begin()),
PINx(*(lst.begin()+1))
{};

C_Pin::C_Pin(GPIO_TypeDef *GPIOx, uint32_t pin2N):
PORTx((((uint32_t)GPIOx) - GPIOA_BASE)/(GPIOB_BASE-GPIOA_BASE)),
PINx(__builtin_ctz(pin2N))
{};

GPIO_TypeDef* C_Pin::GPIOx()
{
	return (GPIO_TypeDef *)(GPIOA_BASE + ((uint32_t)(GPIOB_BASE-GPIOA_BASE))*PORTx);
}

uint16_t C_Pin::Pin2N()
{
	return 1UL<<PINx;
}

void C_Pin::write_pin(PinState x)
{
	GPIO_TypeDef* gpiox = GPIOx();
	uint32_t pin2n = Pin2N();
	if(x){
		gpiox->BSHR = pin2n;
	}else{
		gpiox->BCR = pin2n;
	}
}

PinState C_Pin::read_pin()
{
	return (PinState)GPIO_ReadInputDataBit(GPIOx(), Pin2N());
}

void C_Pin::toggle_pin()
{
	GPIO_TypeDef* gpiox = GPIOx();
	uint32_t pin2n = Pin2N();
	if(gpiox->OUTDR & pin2n){
		gpiox->BCR = pin2n;
	}else{
		gpiox->BSHR = pin2n;
	}
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
	while(((read_pin() xor state) and n<M) or n<m){
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

void C_Pin::loadCfg(PinCfg cfg)
{
	switch(cfg){
	case Pin_PP0:
		loadXCfg(GPIO_GP_PP0);
		break;
	case Pin_PP1:
		loadXCfg(GPIO_GP_PP1);
		break;
	case Pin_OD0:
		loadXCfg(GPIO_GP_OD0);
		break;
	case Pin_OD1:
		loadXCfg(GPIO_GP_OD1);
		break;
	case Pin_InUp:
		loadXCfg(GPIO_In_Up);
		break;
	case Pin_InFlt:
		loadXCfg(GPIO_In_Float);
		break;
	case Pin_InDown:
		loadXCfg(GPIO_In_Down);
		break;
	default:
		break;
	}
}

void C_Pin::loadXCfg(PinXCfg cfg)
{
    GPIO_TypeDef *gpiox = GPIOx();
    uint32_t pin2N = Pin2N();

    //config CNF and MODE on CRL/CRH
    __IO uint32_t *CRx = (PINx < 8) ? &gpiox->CFGLR : &gpiox->CFGHR;
    //if(((uint32_t)cfg) & CNF_MODE_Msk != CNF_MODE_KEEP){ //test faild
        uint32_t reg_offset = (PINx & 0x07) << 2u;
        MODIFY_REG((*CRx), (CNF_MODE_Msk << reg_offset), ((cfg&CNF_MODE_Msk) << reg_offset));
    //}//else{keep old config};
    
    //config ODR
    if(cfg & ODR_Msk){
        gpiox->BSHR = pin2N;
    }else{
        gpiox->BCR = pin2N;
    }
}

#endif
