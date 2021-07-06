/*
 * bmp280.hpp
 *
 *  Created on: Jun 30, 2021
 *      Author: xrj
 */

#ifndef EXTS_INC_BMP280_HPP_
#define EXTS_INC_BMP280_HPP_

//the driver is test failed

#include "myints.h"
#include "s_i2c.hpp"

#define BMP_I2C_Dev S_I2C_Dev

typedef enum{
	BMP280_SleepMode = 0,
	BMP280_ForcedMode = 1,
	BMP280_NormalMode = 3
}BMP280_Mode;

typedef enum{
	BMP280_TSB_0P5MS = 0,
	BMP280_TSB_62P5MS,
	BMP280_TSB_125MS,
	BMP280_TSB_250MS,
	BMP280_TSB_500MS,
	BMP280_TSB_1000MS,
	BMP280_TSB_2000MS,
	BMP280_TSB_4000MS
}BMP280_TSB;

//#pragma pack(1)
typedef struct{
	BMP280_TSB t_sb:3;
	unsigned int filter:3;
	unsigned int pad:1;
	bool spi3w_en:1;
}BMP280_Config;

typedef struct{
	unsigned int osrs_t:3;
	unsigned int osrs_p:3;
	BMP280_Mode mode:2;
}BMP280_CtrlMeas;

class BMP280_Calib{
private:
	u16 dig_T1;
	i16 dig_T2;
	i16 dig_T3;
	u16 dig_P1;
	u16 dig_P2;
	u16 dig_P3;
	u16 dig_P4;
	u16 dig_P5;
	u16 dig_P6;
	u16 dig_P7;
	u16 dig_P8;
	u16 dig_P9;
	i32 t_fine;
public:
	BMP280_Calib();
	void load_defalut();
	u8* get_ptr();
#ifdef USE_FLOAT
	float temp(i32 adc_T);
	float press(i32 adc_P);
#else
	i32 temp(i32 adc_T);
	u32 press(i32 adc_P);
#endif
};
//#pragma pack()

i32 Bigend_20b(u8* data);

class BMP280{
private:
	BMP_I2C_Dev *i2c;
	BMP280_Calib calib;
public:
	u8 data[6];
	BMP280(BMP_I2C_Dev *i2c);
	void Init();
	void reset();
	void set_config(BMP280_Config *cfg);
	void set_ctrl_meas(BMP280_CtrlMeas *cm);
	void read_data();
	i32 calc_temp();
	u32 calc_press();
};


#endif /* EXTS_INC_BMP280_HPP_ */
