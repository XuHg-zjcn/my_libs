/*
 * dc_motor.hpp
 *
 *  Created on: 2021年5月28日
 *      Author: xrj
 */

#include "c_pwm.hpp"
#if !defined(__DC_MOTOR_HPP__) && \
	defined(__C_PWM_HPP__)
#define __DC_MOTOR_HPP__

#include "mylibs_config.hpp"
#include "ssd1306.hpp"


//请参考电路
//TODO: PID控制器类，带异常检测

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

//TODO: 缓启动
class DC_Motor{
private:
	C_PWM_CH *pwm;      //PWM输出
	//CTIM_CH tim_spd;   //测速输入
	uint32_t count;     //总脉冲计数
#ifdef USE_ADC
	C_ADC *cadc;       //ADC
	CADC_CHx CH_Current;  //电流测量
	CADC_CHx CH_Voltage;  //电压测量
#endif
public:
#ifdef USE_ADC
	DC_Motor(CTIM_CH &tim_pwm, C_ADC *cadc, CADC_CHx CH_Current);
#else
	DC_Motor(C_PWM_CH *pwm);
#endif
	void setDuty(float duty);
	void stop();
	void run_pwm();
#ifdef USE_ADC
	u32 t_value(BuffHeadRead head);
	void run_monitor(ControlConfig &cfg, SSD1306 &oled);
#endif
	friend class TestSave;
};

#pragma pack(2)
typedef struct{
	u16 curr;
	u16 spd;
}TestRes;
#pragma pack()

#define MEAN_Msk 0x0fff
#define STD_Msk  0xf000
#define MEAN_Sft      0
#define STD_Sft      12
#define GET_MEAN(x) ((x&MEAN_Msk)>>MEAN_Sft)
#define GET_STD(x)  ((x&STD_Msk)>>STD_Sft)

/*class TestSave{
private:
	DC_Motor *motor;
	TestRes *addr;
	//test param
	u32 n;
	u32 ms;
	float maxduty;
	//hardware param
	float LSB_V;
	float ADCv_I0;
	float LSB_mA;
	u32 pulse_cyc;
	//result
	float tVolt;  //电压时间常数
	float tCurr;  //电流时间常数
	float tMotor; //机械时间常数
	float I_duty; //电流与占空比关系
public:
	TestSave(DC_Motor *motor, TestRes *addr);
	TestSave(DC_Motor *motor, TestRes *addr, u32 n, u32 ms, float maxduty);
	void InitHWParam(float LSB_V, float ADCv_I0, float LSB_mA, u32 pulse_cyc);
	void TestStart();
	void Eraser();
	TestRes get(float duty);
};*/

#endif /* __DC_MOTOR_HPP__ */
