/*
 * bmp280.hpp
 *
 *  Created on: Jun 30, 2021
 *      Author: xrj
 */

#ifndef EXTS_INC_BMP280_HPP_
#define EXTS_INC_BMP280_HPP_


#include "myints.h"
#include "c_i2c.hpp"

#define BMP_I2C_Dev C_I2C_Dev
#define USE_INTERGER

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
typedef struct bmp_cfg{
	bool spi3w_en:1;
	unsigned int pad:1;
	unsigned int filter:3;
	BMP280_TSB t_sb:3;
	bmp_cfg(BMP280_TSB t_sb, u8 filter, bool spi3w_en):spi3w_en(spi3w_en),pad(0),filter(filter),t_sb(t_sb){};
}BMP280_Config;

typedef struct bmp_ctrlmeas{
	BMP280_Mode mode:2;
	unsigned int osrs_p:3;
	unsigned int osrs_t:3;
	bmp_ctrlmeas(u8 osrs_t, u8 osrs_p, BMP280_Mode mode):mode(mode),osrs_p(osrs_p),osrs_t(osrs_t){};
}BMP280_CtrlMeas;

class BMP280_Calib{
private:
	u16 dig_T1;
	i16 dig_T2;
	i16 dig_T3;
	u16 dig_P1;
	i16 dig_P2;
	i16 dig_P3;
	i16 dig_P4;
	i16 dig_P5;
	i16 dig_P6;
	i16 dig_P7;
	i16 dig_P8;
	i16 dig_P9;
	i32 t_fine;
public:
	BMP280_Calib();
	void load_defalut();
	u8* get_ptr();
#ifdef USE_FLOAT
	float temp(i32 adc_T);
	float press(i32 adc_P);
#endif
#ifdef USE_INTERGER
	i32 temp(i32 adc_T);
	u32 press(i32 adc_P);
#endif
};
//#pragma pack()

i32 Bigend_20b(u8* data);

class BMP280{
private:
	BMP_I2C_Dev *i2c;
#if defined(USE_FLOAT) || defined(USE_INTERGER)
	BMP280_Calib calib;
#endif
public:
#if defined(USE_FLOAT) || defined(USE_INTERGER)
	u8 data[6];
	X_State read_data();
#endif
	BMP280(BMP_I2C_Dev *i2c);
	void Init();
	X_State read_calib(u8* calib);
	void reset();
	void set_config(BMP280_Config cfg);
	void set_ctrl_meas(BMP280_CtrlMeas cm);
	X_State read_data(u8* data);
#ifdef USE_INTERGER
	i32 calc_temp();
	u32 calc_press();
#endif
};


#endif /* EXTS_INC_BMP280_HPP_ */
