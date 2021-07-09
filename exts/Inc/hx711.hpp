#ifndef __HX711__
#define __HX711__

#include "myints.h"
#include "c_pin.hpp"

typedef enum{
	HX711_CHA_128 = 1,
	HX711_CHB_32 = 2,
	HX711_CHA_64 = 3
}HX711_Conv;

class HX711{
private:
	C_Pin sck;
	C_Pin dout;
	u32 loops;
	i32 val_at0;
	float kg_LSB;
	HX711_Conv mode;
public:
	HX711(C_Pin sck, C_Pin dout);
	void Init(HX711_Conv mode);
	void wait();
	i32 read_raw(HX711_Conv next);
	i32 block_raw();
	float read_kg();
};

#endif
