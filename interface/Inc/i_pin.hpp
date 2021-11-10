/*
 * c_gpio.hpp
 *
 *  Created on: 2021年6月13日
 *      Author: xrj
 */

#ifndef COMMON_INC_I_PIN_HPP_
#define COMMON_INC_I_PIN_HPP_

#include "myints.h"

typedef enum{
    Pin_PP0 = 0,
    Pin_PP1,
    Pin_OD0,
    Pin_OD1,
    Pin_InUp,
    Pin_InFlt,
    Pin_InDown
}PinCfg;

#define PIN_IS_OUT(x) (x<=Pin_OD1)
#define PIN_IS_IN(x)  (x>=Pin_InUp)

#define NEAREST_PINSPEEDD(x) Pin_SpdDefa

//TODO: 改为Pin_Low, Pin_High
typedef enum{
    Pin_Reset = 0,
    Pin_Set
}PinState;

inline PinState operator!(PinState s){
	return (PinState)(!s);
}

//TODO: 应用16位引脚配置结构体
typedef struct{
//上下拉MOS管
    bool upMOS:1;
    bool dnMOS:1;
//上下拉电阻
    bool upRes:1;
    bool dnRes:1;
//中断
    bool RiseInt:1; //上升沿/高电平中断
    bool FallInt:1; //下降沿/低电平中断
    bool LevelInt:1;//电平中断模式
//其他1bit设置
    bool DigIn:1;    //STC单片机开启数字输入，STM8/32切换模拟模式和输入模式
    bool Schmitt:1;  //施密特触发器，STC单片机可设置
    bool afio:1;     //功能复用，STM8/32有效
    PinState InitLevel:1;//初始化输出电平
//多bit设置
    u8   DrMos:2;  //MOS管驱动能力
    u8   DrRes:1;  //电阻驱动能力
    u8   Speed:2;  //输出速度
}PinCfg16b;

class I_Pin{
public:
//需实现
    void write_pin(PinState x);
    PinState read_pin();
    void loadCfg(PinCfg cfg, u8 MHz);
    void loadCfg(PinCfg cfg);
//已实现，可覆盖
    void toggle_pin();
    void wait_pin(PinState state);
    u32 wait_timeout(PinState state, u32 timeout);
    u32 wait_count(PinState state, u32 m, u32 M);
};

#endif /* COMMON_INC_I_PIN_HPP_ */
