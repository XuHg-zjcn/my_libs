/*
 * htu2x.hpp
 *
 *  Created on: 2021年11月12日
 *      Author: xrj
 */

#include "c_i2c.hpp"
#if !defined(__HTU2X_HPP__) && \
	defined(__C_I2C_HPP__)
#define __HTU2X_HPP__

#include "mylibs_config.hpp"

#define HTU_I2C_Dev C_I2C_Dev
#define HTU_Addr 0x40
#define USE_BUFF
//#define HOLD_READ  //HOLD模式测试失败，请不要使用

typedef enum{
	HTU_OK = 0,
	HTU_Busy,
	HTU_CRCErr,
}HTU_Stat;

typedef enum{
	HTU_RH12_Tmp14 = 0,
	HTU_RH8_Tmp12,
	HTU_RH10_Tmp13,
	HTU_RH11_Tmp11
}HTU_Resol;

typedef struct{
	HTU_Resol resol:4;
	bool EndofBattery:1;
	bool resv:1;
	bool heater:1;
	bool disable_OTP:1;
}HTU2x_UserReg;

typedef enum{
	HTU_Temp_Hold = 0xE3,
	HTU_Hum_Hold = 0xE5,
	HTU_Temp_NoHold = 0xF3,
	HTU_Hum_NoHold = 0xF5,
	HTU_Write_UserReg = 0xE6,
	HTU_Read_UserReg = 0xE7,
	HTU_Soft_Reset = 0xFE
}HTU2x_Commd;

class HTU2x{
private:
	HTU_I2C_Dev *i2c;
#ifdef HOLD_READ
	u16 Hold_Read(HTU2x_Commd commd);
#endif
	u16 NoHold_Read(HTU2x_Commd commd, uint32_t wait);
public:
	HTU2x(HTU_I2C_Dev *i2c);
	i16 Read_Temp0p01C();
	i16 Read_Hum0p01();
	void Write_UserReg(HTU2x_UserReg reg);
	HTU2x_UserReg Read_UserReg();
};

#endif /* __HTU2X_HPP__ */
