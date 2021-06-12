#ifndef PINS_MANAGER_HPP
#define PINS_MANAGER_HPP

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
	C_Pin p8b;
	bool keep:1;    //keep use flag in disable, note: will still load CfgDisable when GPIO_Conn entry disable state.
	InitCfg cfg0:2; //which cfg in init
	PinLockType lock:2; //lock pin type
	PinCfg CfgEnable:8;
	PinCfg CfgDisable:8;
}ManagerPin;

//TODO: auto config periph during enable.
class GPIO_Conn{
private:
	ManagerPin* pins;
	uint32_t N_pin;
public:
	GPIO_Conn(ManagerPin* pins, uint32_t N_pin);
	GPIO_Conn(C_Pin* p8b, uint32_t N_pin, bool keep,
			InitCfg cfg0, PinLockType lock,
			PinCfg CfgEnable, PinCfg CfgDisable);
	bool isAvailable();
	X_State Enable();
	X_State Disable();
	ManagerPin* operator[](int i);
	void WritePins(uint32_t bits);
	uint32_t ReadPins();
};

inline bool isPinUsed(C_Pin pin);
inline void setPinUsed(C_Pin pin, bool used);

#endif
