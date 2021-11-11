/*
 * aht2x.hpp
 *
 *  Created on: 2021年11月11日
 *      Author: xrj
 */

#ifndef EXTS_INC_AHT2X_HPP_
#define EXTS_INC_AHT2X_HPP_

#include "mylibs_config.hpp"
#include "c_i2c.hpp"

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


#endif /* EXTS_INC_AHT2X_HPP_ */
