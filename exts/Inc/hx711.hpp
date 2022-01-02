#include "c_pin.hpp"
#if !defined(__HX711_HPP__) && \
	defined(__C_PIN_HPP__)
#define __HX711_HPP__

#include "myints.h"

typedef enum{
	HX711_CHA_128 = 1,
	HX711_CHB_32 = 2,
	HX711_CHA_64 = 3
}HX711_Conv;

class HX711{
private:
	u32 loops;
	i32 val_at0;
	float kg_LSB;
	HX711_Conv mode;
public:
	C_Pin sck;
	C_Pin dout;
	HX711(C_Pin sck, C_Pin dout);
	void Init(HX711_Conv mode);
	void wait();
	i32 read_raw(HX711_Conv next);
	i32 block_raw();
	float read_kg();
};

#endif /* __HX711_HPP__ */
