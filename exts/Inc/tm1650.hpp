/*
 * tm1650.hpp
 *
 *  Created on: Jul 1, 2021
 *      Author: xrj
 */

#ifndef EXTS_INC_TM1650_HPP_
#define EXTS_INC_TM1650_HPP_

#include "smg8.hpp"

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
	C_I2C *i2c;
public:
	TM1650(I2C_HandleTypeDef *i2c);
	void setDig(u32 i, u8 data);            //0x6(8/A/C/E)
	u8 readKey(u32 abc, u32 dig);
	void send_commad(TM1650_Commands cmd); //0x48 cmd
	void off();
};


#endif /* EXTS_INC_TM1650_HPP_ */
