/*
 * tm1650.hpp
 *
 *  Created on: Jul 1, 2021
 *      Author: xrj
 */

#ifndef EXTS_INC_TM1650_HPP_
#define EXTS_INC_TM1650_HPP_

#include "smg8.hpp"
#include "s_i2c.hpp"

#define TM_I2C S_I2C //I2C类型选择 建议使用软件I2C

/*
 * 注意: TM1650只能驱动7段或8段的共阴数码屏，最多只能驱动4个，
 *      时间数码管的冒号可单独引出或连接DP，详细见数据手册。
 */

#pragma pack(1)
typedef struct sCommands{
	bool on:1;
	unsigned int pad2:2;
	bool isSMG7:1;
	unsigned int level:3;
	unsigned int pad1:1;
	sCommands(u8 level, bool isSMG7, bool on):
		on(on),pad2(0),isSMG7(isSMG7),level(level),pad1(0){};
}TM1650_Commands;
#pragma pack()

/*
 * TODO: test problem
 * bit '0' is a little dark than '1', difficult to see difference.
 */
class TM1650 : public SMG8{
private:
	TM_I2C *i2c;
public:
	TM1650(TM_I2C *i2c);
	void setDig(u32 i, u8 data);            //0x6(8/A/C/E)
	u8 readKey(u32 abc, u32 dig);
	void send_commad(TM1650_Commands cmd); //0x48 cmd
	void off();
};


#endif /* EXTS_INC_TM1650_HPP_ */
