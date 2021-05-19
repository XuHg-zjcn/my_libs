uint16_t pins_use_state[16];  //TODO: use FreeRTOS event flags, can wait

inline bool isPinUsed(Pin_8b pin)
{
    if(pins_use_state[pin.PORTx] & 1<<pin.PINx){
        return true;
    }else{
        return false;
    }
}

inline void setPinUsed(Pin_8b pin, bool used)
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
    for(int i=0;i<N_pin;i++){
        if(p1->keep | p1->cfg0==InitCfg_Enable){
            if(isPinUsed(p1->p8b)){
                Error_Handle();
            }else{
                setPinUsed(p1->p8b, true);
            }
        }
        switch(p1->cfg0){
            case InitCfg_Empty:
                break;
            case InitCfg_Enable:
                loadPinCfg(p1->p8b, p1->CfgEnable);
                break;
            case InitCfg_Disable:
                loadPinCfg(p1->p8b, p1->CfgDisable);
                break;
            default:
                break;
        }
        if(p1->lock == LockAtInitConn){
            LockPin(p1->p8b);
        }
        p1++;
    }
}

bool GPIO_Conn::isAvailable()
{
    ManagerPin* p1 = pins;
    for(int i=0;i<N_pin;i++){
        if(!p1->keep && isPinUsed(p1->p8b)){ // use flag not keep && already use
            return false;
        }
    }
    return true;
}

X_State GPIO_Conn::Enable()
{
    if(!isAvailable()){
        return X_Busy;
    }
    for(int i=0;i<N_pin;i++){
        loadPinCfg(pins->p8b, pins->CfgEnable);
        if(!pins->keep){
            setPinUsed(pins->p8b, true);
        }
    }
    return X_OK;
}

X_State GPIO_Conn::Enable()
{
    for(int i=0;i<N_pin;i++){
        loadPinCfg(pins->p8b, pins->CfgDisable);
    }
    return X_OK;
}