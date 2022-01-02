/*
 * aht2x.hpp
 *
 *  Created on: 2021年11月11日
 *      Author: xrj
 */

#include "c_i2c.hpp"
#if !defined(__AHT2X_HPP__) && \
	defined(__C_I2C_HPP__)
#define __AHT2X_HPP__

#define AHT_I2C_Dev C_I2C_Dev

typedef enum{
	AHT_OK = 0,
	AHT_Busy,
	AHT_CRCErr,
}AHT_Stat;

class AHT2x{
private:
	AHT_I2C_Dev *i2c;
	u8 data[7];
public:
	AHT2x(AHT_I2C_Dev *i2c);
	AHT_Stat Init();
	u8 CRC8();
	void trig();
	AHT_Stat read();
	AHT_Stat trig_read();
	i32 mCTemp();
	u32 mHum();
};

#endif /* __AHT2X_HPP__ */
