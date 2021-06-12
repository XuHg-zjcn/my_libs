#ifndef PINS_MANAGER_HPP
#define PINS_MANAGER_HPP

#include "x_base.hpp"
#include "mylibs_config.hpp"


#pragma pack(1)
//Pin用途接口类型, 每种类型都难以共用同一个引脚
typedef enum{
	Pin_GPIO=0, //包含EXTI
	Pin_UART,
	Pin_SPI,    //不包含CS引脚
	Pin_I2C,
	Pin_SDIO,
	Pin_USB,
	Pin_TIM,
	Pin_ADC,
	Pin_OSC
}PinFunc;

//接口实现类型
typedef enum{
	PinFunc_Software=0,
	PinFunc_Hardware
}PinImpType;

typedef struct{
	unsigned int PinOut:1;
	unsigned int PinIn:1;
	unsigned int PinAna:1;
	PinImpType imp:1;
}PinBits;

typedef struct{
	PinFunc func:8;
	PinBits bits:8;
	unsigned int dev_id:4;
	unsigned int pin_id:4;
}PinSave;
#pragma pack()

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

//TODO: PinCfg each connect, not each pin
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
