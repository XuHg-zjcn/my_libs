/*
 * bmp280.cpp
 *
 *  Created on: Jun 30, 2021
 *      Author: xrj
 */

#include "bmp280.hpp"
#include "delay.hpp"

#define BMP280_S32_t int32_t
#define BMP280_S64_t int64_t
#define BMP280_U32_t uint32_t
#define BMP280_U64_t uint64_t

BMP280_Calib::BMP280_Calib()
{
	load_defalut();
}

void BMP280_Calib::load_defalut()
{
	dig_T1 = 27504;
	dig_T2 = 26435;
	dig_T3 = -1000;
	dig_P1 = 36477;
	dig_P2 = -10685;
	dig_P3 = 3024;
	dig_P4 = 2855;
	dig_P5 = 140;
	dig_P6 = -7;
	dig_P7 = 15500;
	dig_P8 = -14600;
	dig_P9 = 6000;
}

u8* BMP280_Calib::get_ptr()
{
	return (u8*)this;
}


#ifdef USE_FLOAT
//code ocr from datasheet page 23
float BMP280_Calib::temp(BMP280_S32_t adc_T)
{
	float var1, var2;
	var1 = (((float)adc_T)/16384.0f - ((float)dig_T1)/1024.0f) * ((float)dig_T2);
	var2 = ((((float)adc_T)/131072.0f - ((float)dig_T1)/8192.0f) * (((float)adc_T)/131072.0f - ((float) dig_T1)/8192.0f)) * ((float)dig_T3);	t_fine = (BMP280_S32_t)(var1 + var2);
	t_fine = (BMP280_S32_t)(var1 + var2);
	return (var1 + var2)/5120.0f;
}

float BMP280_Calib::press(BMP280_S32_t adc_P)
{
	float var1, var2, p;
	var1 = ((float)t_fine/2.0f) - 64000.0f;
	var2 = var1 * var1 * ((float)dig_P6) / 32768.0f;
	var2 = var2 + var1* ((float)dig_P5) * 2.0f;
	var2 = (var2/4.0)+(((float)dig_P4) * 65536.0f);
	var1 = (((float)dig_P3) * var1 * var1 / 524288.0f + ((float)dig_P2) * var1) / 524288.0f;
	var1 = (1.0f + var1/32768.0f)*((float)dig_P1);
	p = 1048576.0f - (float)adc_P;
	p = (p - (var2 / 4096.0f)) * 6250.0f / var1;
	var1 = ((float)dig_P9) * p * p / 2147483648.0f;
	var2 = p * ((float)dig_P8) / 32768.0f;
	return p + (var1 + var2 + ((float)dig_P7)) / 16.0f;
}
#else
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP280_S32_t BMP280_Calib::temp(BMP280_S32_t adc_T)
{
	BMP280_S32_t var1, var2, T;
	var1 = ((((adc_T>>3) - ((BMP280_S32_t)dig_T1<<1))) * ((BMP280_S32_t)dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((BMP280_S32_t)dig_T1)) * ((adc_T>>4) - ((BMP280_S32_t)dig_T1))) >> 12) *
	((BMP280_S32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}
// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BMP280_U32_t BMP280_Calib::press(BMP280_S32_t adc_P)
{
	BMP280_S64_t var1, var2, p;
	var1 = ((BMP280_S64_t)t_fine) - 128000;
	var2 = var1 * var1 * (BMP280_S64_t)dig_P6;
	var2 = var2 + ((var1*(BMP280_S64_t)dig_P5)<<17);
	var2 = var2 + (((BMP280_S64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (BMP280_S64_t)dig_P3)>>8) + ((var1 * (BMP280_S64_t)dig_P2)<<12);
	var1 = (((((BMP280_S64_t)1)<<47)+var1))*((BMP280_S64_t)dig_P1)>>33;
	if (var1 == 0)
	{
	return 0; // avoid exception caused by division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((BMP280_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((BMP280_S64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)dig_P7)<<4);
	return (BMP280_U32_t)p;
}
#endif

#if defined(USE_FLOAT) || defined(USE_INTERGER)
BMP280::BMP280(BMP_I2C_Dev *i2c):calib()
#else
BMP280::BMP280(BMP_I2C_Dev *i2c)
#endif
{
	this->i2c = i2c;
}

void BMP280::Init()
{
#if defined(USE_FLOAT) || defined(USE_INTERGER)
	i2c->Mem_read(0x88, calib.get_ptr(), 24);
#endif
	reset();
	XDelayMs(20);
	set_config(BMP280_Config(BMP280_TSB_0P5MS, 5, false));
	set_ctrl_meas(BMP280_CtrlMeas(5, 5, BMP280_NormalMode));
}

X_State BMP280::read_calib(u8* calib)
{
	return i2c->Mem_read(0x88, calib, 24);
}

void BMP280::reset()
{
	u8 byte=0xb6;
	i2c->Mem_write(0xe0, &byte, 1);
}

void BMP280::set_config(BMP280_Config cfg)
{
	i2c->Mem_write(0xF5, (u8*)&cfg, 1);
}

void BMP280::set_ctrl_meas(BMP280_CtrlMeas cm)
{
	i2c->Mem_write(0xF4, (u8*)&cm, 1);
}

#if defined(USE_FLOAT) || defined(USE_INTERGER)
X_State BMP280::read_data()
{
	return i2c->Mem_read(0xF7, data, 6);
}
#endif

X_State BMP280::read_data(u8* data)
{
	return i2c->Mem_read(0xF7, data, 6);
}

i32 Bigend_20b(u8* p)
{
	i32 ret;
	ret = (*p++)<<12;
	ret |= (*p++)<<4;
	ret |= (*p)>>4;
	return ret;
}

#if defined(USE_FLOAT) || defined(USE_INTERGER)
i32 BMP280::calc_temp()
{
	i32 adc_T = Bigend_20b(data+3);
	return calib.temp(adc_T);
}

u32 BMP280::calc_press()
{
	i32 adc_P = Bigend_20b(data);
	return calib.press(adc_P);
}
#endif
