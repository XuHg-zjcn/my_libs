#ifndef PIN_MANAGER_HPP
#define PIN_MANAGER_HPP

#include "x_base.hpp"
#include "mylibs_config.hpp"

typedef enum{
	NoLock = 0,
	LockAtFirstOn,
	LockAtInitConn
}PinLockType;

typedef enum{
	InitCfg_Empty = 0,
	InitCfg_Enable,
	InitCfg_Disable,
}InitCfg;

typedef struct{
	Pin8b p8b;
	bool keep:1;    //keep use flag in disable, note: will still load CfgDisable when GPIO_Conn entry disable state.
	InitCfg cfg0:2; //which cfg in init
	PinLockType lock:2; //lock pin type
	PinCfg CfgEnable;
	PinCfg CfgDisable;
}ManagerPin;

//TODO: auto config periph during enable.
class GPIO_Conn{
private:
	ManagerPin* pins;
	uint32_t N_pin;
public:
	GPIO_Conn(ManagerPin* pins, uint32_t N_pin);
	bool isAvailable();
	X_State Enable();
	X_State Disable();
};

inline bool isPinUsed(Pin8b pin);
inline void setPinUsed(Pin8b pin, bool used);

#endif
