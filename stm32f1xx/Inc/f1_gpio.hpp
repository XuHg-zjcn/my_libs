#ifndef F1_GPIO_HPP
#define F1_GPIO_HPP

#include "mylibs_config.hpp"
#include "stm32f1xx_hal.h"
#include "myints.h"
#include <map>


typedef enum{
    Pin_Reset = 0,
    Pin_Set
}PinState;


//order below same to Reference manual RM0008, Table20
//Output are 50MHz, please use0826543 '&' to set Speed, example: `GPIO_GP_PP0 & OUT_2MHZ`
typedef enum{  //ODR 1b|CNF 2b|MODE 2b|
    GPIO_GP_PP0   = 0b00011,
    GPIO_GP_PP1   = 0b10011,
    GPIO_GP_OD0   = 0b00111,
    GPIO_GP_OD1   = 0b10111,
    GPIO_AF_PP    = 0b01011,
    GPIO_AF_OD    = 0b01111,
    GPIO_Analog   = 0b00000,
    GPIO_In_Float = 0b00100,
    GPIO_In_Down  = 0b01000,
    GPIO_In_Up    = 0b11000,
    GPIO_Keep_ODR0= 0b01100,
    GPIO_Keep_ODR1= 0b11100
}PinCfg;

typedef enum{
	Out_10MHz = (~0b11 | 0b01),
	Out_2MHz  = (~0b11 | 0b10),
	Out_50MHz = (~0b11 | 0b11)
}OutSpeed;

inline PinState operator!(PinState s){
	return (PinState)(!s);
}

inline PinCfg operator&(PinCfg cfg, OutSpeed spd){
	return (PinCfg)(cfg&spd);
}

#define MODE_Msk      0b00011
#define MODE_IN       0b00000

#define CNF_Msk       0b01100
#define CNF_PPOD_Msk  0b00100
#define CNF_GPAF_Msk  0b01000

#define CNF_MODE_Msk  0b01111
#define CNF_MODE_KEEP 0b01100

#define ODR_Msk       0b10000

typedef enum{
    EXTI_DISABLE0 = 0,
    EXTI_IT_RISE,
    EXTI_IT_FALL,
    EXTI_IT_RISE_FALL,
    EXTI_DISABLE1,
    EXTI_EVT_RISE,
    EXTI_EVT_FALL,
    EXTI_EVT_RISE_FALL
}EnumEXTI;
#define EXTI_RISE_Msk 0b01
#define EXTI_FALL_Msk 0b10
#define EXTI_EVT_Msk 0b100

//TODO: use enum PinState instead bool

class C_Pin{
public:
    unsigned int PORTx:4;
    unsigned int PINx:4;
    C_Pin(uint32_t port, uint32_t pin);
    C_Pin(std::initializer_list<uint32_t> lst);
    C_Pin(GPIO_TypeDef *GPIOx, uint32_t pin2N);
    GPIO_TypeDef* GPIOx();
    uint32_t Pin2N();
    uint32_t* ODR_bitband();
    uint32_t* IDR_bitband();
    void write_pin(bool x);  //TODO: remove "_pin"
    PinState read_pin();
    void toggle_pin();
    void wait_pin(PinState state);
    u32 wait_timeout(PinState state, u32 timeout);
    u32 wait_count(PinState state, u32 m, u32 M);
    void loadCfg(PinCfg cfg);
    void lockCfg();
    void setEXTI(EnumEXTI exti);
};

#endif
