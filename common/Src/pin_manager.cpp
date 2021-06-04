#include "pins_manager.hpp"
#include "x_logs.hpp"


uint16_t pins_use_state[16];  //TODO: use FreeRTOS event flags, can wait

inline bool isPinUsed(Pin8b pin)
{
    if(pins_use_state[pin.PORTx] & 1<<pin.PINx){
        return true;
    }else{
        return false;
    }
}

inline void setPinUsed(Pin8b pin, bool used)
{
    if(used){
        pins_use_state[pin.PORTx] |= 1<<pin.PINx;
    }else{
        pins_use_state[pin.PORTx] &= ~(1<<pin.PINx);
    }
}

GPIO_Conn::GPIO_Conn(ManagerPin* pins, uint32_t N_pin)
{
    this->pins = pins;
    this->N_pin = N_pin;
    ManagerPin* p1 = pins;
    for(uint32_t i=0;i<N_pin;i++){
        if(p1->keep || p1->cfg0==InitCfg_Enable){
            if(isPinUsed(p1->p8b)){
                X_ErrorLog(__FILE__, __LINE__);
            }else{
                setPinUsed(p1->p8b, true);
            }
        }
        switch(p1->cfg0){
            case InitCfg_Empty:
                break;
            case InitCfg_Enable:
                p1->p8b.loadCfg(p1->CfgEnable);
                break;
            case InitCfg_Disable:
                p1->p8b.loadCfg(p1->CfgDisable);
                break;
            default:
                break;
        }
        if(p1->lock == LockAtInitConn){
            p1->p8b.lockCfg();
        }
        p1++;
    }
}

GPIO_Conn::GPIO_Conn(Pin8b* p8b, uint32_t N_pin, bool keep,
					InitCfg cfg0, PinLockType lock,
					PinCfg CfgEnable, PinCfg CfgDisable)
{
	pins = (ManagerPin*)XMalloc(sizeof(ManagerPin)*N_pin); //TODO: call XFree in ~GPIO_Conn
	ManagerPin *pins2 = pins;
	for(uint32_t i=0;i<N_pin;i++){
		pins2->p8b = *p8b++;
		pins2->keep = keep;
		pins2->cfg0 = cfg0;
		pins2->lock = lock;
		pins2->CfgEnable = CfgEnable;
		pins2->CfgDisable = CfgDisable;
		pins2++;
	}
	new (this)GPIO_Conn(pins, N_pin);
}

bool GPIO_Conn::isAvailable()
{
    ManagerPin* p1 = pins;
    for(uint32_t i=0;i<N_pin;i++){
        if(!p1->keep && isPinUsed(p1->p8b)){ // use flag not keep && already use
            return false;
        }
    }
    return true;
}

X_State GPIO_Conn::Enable()
{
    ManagerPin* p1 = pins;
    if(!isAvailable()){
        return X_Busy;
    }
    for(uint32_t i=0;i<N_pin;i++){
        p1->p8b.loadCfg(p1->CfgEnable);
        if(!p1->keep){
            setPinUsed(p1->p8b, true);
        }
        p1++;
    }
    return X_OK;
}

X_State GPIO_Conn::Disable()
{
    ManagerPin* p1 = pins;
    for(uint32_t i=0;i<N_pin;i++){
        p1->p8b.loadCfg(p1->CfgDisable);
        p1++;
    }
    return X_OK;
}

ManagerPin* GPIO_Conn::operator[](int i)
{
	return &pins[i];
}

//modify ODR
void GPIO_Conn::WritePins(uint32_t bits)
{
	ManagerPin* p1 = pins;
	for(uint32_t i=0;i<N_pin;i++){
		p1++->p8b.write_pin((GPIO_PinState)(bits&0x1));
		bits>>=1;
	}
}

uint32_t GPIO_Conn::ReadPins()
{
	uint32_t ret=0;
	ManagerPin* p1 = pins;
	for(uint32_t i=0;i<N_pin;i++){
		ret |= (p1++->p8b.read_pin())<<i;
	}
	return ret;
}
