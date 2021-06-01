/*
 * dc_motor.hpp
 *
 *  Created on: 2021年5月28日
 *      Author: xrj
 */

#ifndef COMMON_INC_DC_MOTOR_HPP_
#define COMMON_INC_DC_MOTOR_HPP_

#include "mylibs_config.hpp"
#include "c_tim.hpp"
#include "c_adc.hpp"
#include "ssd1306.hpp"


//请参考电路

#define PWM_HZ   20000  //PWM正常频率
#define MIN_NS   10000  //最小电平维持时间，自动降低频率，避免频繁切换导致效率低
#define PULSE_CYC    6  //每圈脉冲个数

#define DUTY_MAX   0.6
#define DUTY_MIN   0.05

#define NZIP  128
#define NSAMP 4096


typedef enum{
	ConsDuty = 0,  // duty*65536
	ConsSpeed,     // r/min
	ConsCurrent,   // mA
	ConsPower      // mW
}ControlVar;

typedef struct{
	bool current_on:1;
	bool voltage_on:1;
	bool speed_tim:1;
	bool speed_curr:1;
	ControlVar con;
	uint32_t value;
}ControlConfig;

class DC_Motor{
private:
	TIM_CH tim_pwm;      //PWM输出
	TIM_CH tim_spd;      //测速输入
	uint32_t count;      //总脉冲计数

	C_ADCEx *hadc;       //ADC
	ADC_CHx CH_Current;  //电流测量
	ADC_CHx CH_Voltage;  //电压测量
public:
	DC_Motor(TIM_CH &tim_pwm, C_ADCEx *hadc, ADC_CHx CH_Current);
	void setDuty(float duty);
	float LTest();  //电感量测试
	float RTest();  //电阻测试
	void wait_OK();
	void run_monitor(ControlConfig &cfg, SSD1306 &oled);
};



#endif /* COMMON_INC_DC_MOTOR_HPP_ */
