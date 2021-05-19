#ifndef F1_GPIO_HPP
#define F1_GPIO_HPP

#include "stm32f1xx_hal.h"

#define TOTAL_PORTS 7 //GPIOx(x=ABCDEFG)

#pragma pack(1)
typedef struct{
    unsigned int PORTx:4;
    unsigned int PINx:4;
}Pin_8b;

#define p8b_GPIOx(x)                   ((GPIO_TypeDef *)(GPIOA_BASE + 0x400*x.PORTx))
#define p8b_Pin2N(x)                   (1<<x.PINx)
#define GPIOx_P2N_to_p8b(GPIOx, Pin2N) Pin_8b{(uint32)&GPIOx - GPIOA_BASE, __builtin_ctz(Pin2N)}


typedef enum{
    Input = 0,
    Out_10MHz,
    Out_2MHz,
    Out_50MHz
}EnumMode;

typedef enum{
    Analog_GPPP = 0,
    Floating_GPOD,
    InputPull_AFPP,
    KeepCfg_AFOD
}EnumCNF;
#define CNF_PPOD_Msk 0b01
#define CNF_GPAF_Msk 0b10

//order below same to Reference manual RM0008, Table20
//Output are 50MHz, please use0826543 '&' to set Speed, example: `GPIO_GP_PP0 & OUT_2MHZ`
typedef enum{  //|ODR 1b|CNF 2b|MODE 2b|
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
    GPIO_Keep_ODR1= 0b11100,
}EnumPinCfg;
#define OUT_10MHZ (~0b11 | Out_10MHz)
#define OUT_2MHZ  (~0b11 | Out_2MHz)
#define OUT_50MHZ (~0b11 | Out_50MHz)

#define CNF_MODE_Msk  0b1111
#define CNF_MODE_KEEP 0b1100


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

//Cfg of a Pin
typedef struct{
    EnumMode mode:2;
    EnumCNF cnf:2;
    unsigned int odr:1;
    EnumEXTI exti:3;
}StruPinCfg;

typedef union{
    StruPinCfg s;
    EnumPinCfg e;
}UnionPinCfg;
#pragma pack()

void loadPinCfg(Pin_8b pin, UnionPinCfg cfg);
void lockPinCfg(Pin_8b pin);
void convertTo_HAL_GPIO_Init(UnionPinCfg cfg, GPIO_InitTypeDef *HAL_GPIO_Init);
UnionPinCfg convertFrom_HAL_GPIO_Init(GPIO_InitTypeDef *HAL_GPIO_Init);

#endif
