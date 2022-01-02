/*
 * dht11.hpp
 *
 *  Created on: 2021年6月7日
 *      Author: xrj
 */


#include "c_pin.hpp"
#if !defined(__DHT11_HPP__) && \
	defined(__C_PIN_HPP__)
#define __DHT11_HPP__

#include "mylibs_config.hpp"

//#define USE_VAR_SPLIT  //measure time of high level by loop
//TODO: measure time of high level by hardware timer, is no affect of interrupt.

#define MIN_LOOPS    5
#define MAX_LOOPS 1000
#define WAIT(x) pin.wait_timeout(x, MAX_LOOPS)
//#define WAIT(x) pin.wait_pin(x)

#define LOW_MS 20
#define SPILT01_US 47  //not need in `USE_VAR_SPLIT` mode

#define DHT_INTEGER
//#define DHT_FLOAT

typedef enum{
	DHT11_OK = 0,
	DHT11_RxTimeout = -1,
	DHT11_CheckSum_Err = -2
}DHT11_PackState;

#pragma pack(1)
typedef struct{
	u8 hum_H;
	u8 hum_L;
	u8 temp_H;
	u8 temp_L;
	u8 check;
	DHT11_PackState ps;
}DHT11_RAW;
#pragma pack()

class DHT11{
private:
	C_Pin pin;
	DHT11_RAW raw;
#ifdef USE_VAR_SPLIT
	u32 split;
#endif
	i32 read_byte();
public:
	DHT11(C_Pin pin);
	DHT11_PackState read_raw();
#ifdef USE_VAR_SPLIT
	void test(u32 *tH);
	u32 carlib_split();
#endif
#ifdef DHT_INTEGER
	u16 hum_0p1();
	i16 temp_0p1();
#endif
#ifdef DHT_FLOAT
	float hum();
	float temp();
#endif
};

#endif /* __DHT11_HPP__ */
