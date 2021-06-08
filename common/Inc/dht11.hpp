/*
 * dht11.hpp
 *
 *  Created on: 2021年6月7日
 *      Author: xrj
 */

#ifndef COMMON_INC_DHT11_HPP_
#define COMMON_INC_DHT11_HPP_

#include "mylibs_config.hpp"

#define USE_VAR_SPLIT  //measure time of high level by loop
//TODO: measure time of high level by hardware timer, is no affect of interrupt.

#define MIN_LOOPS    5
#define MAX_LOOPS 1000
#define WAIT(x) pin.wait_timeout(x, MAX_LOOPS)
//#define WAIT(x) pin.wait_pin(x)

#define LOW_MS 20
#define SPILT01_US 47  //not need in `USE_VAR_SPLIT` mode

typedef enum{
	DHT11_OK,
	DHT11_RxTimeout,
	DHT11_CheckSum_Err
}DHT11_PackState;

#pragma pack(1)
typedef struct{
	u8 check;
	u8 temp_L;
	u8 temp_H;
	u8 hum_L;
	u8 hum_H;
	DHT11_PackState ps;
}DHT11_RAW;
#pragma pack()

class DHT11{
private:
	Pin8b pin;
#ifdef USE_VAR_SPLIT
	u32 split;
#endif
	DHT11_PackState read_bits(u32 *bb, u32 n);
	u8 read_byte();
public:
	DHT11(Pin8b *pin);
	DHT11_PackState read_raw(DHT11_RAW *data);
	void test(u32 *tH);
#ifdef USE_VAR_SPLIT
	u32 carlib_split();
#endif
	static float hum(DHT11_RAW *data);
	static float temp(DHT11_RAW *data);
};


#endif /* COMMON_INC_DHT11_HPP_ */
