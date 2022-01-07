#include "app.h"
#include "c_pin.hpp"
#include "delay.hpp"


int app(void)
{
	C_Pin led = C_Pin(2, 13);
	led.loadXCfg(GPIO_PP_PP0);
	while(1){
		led.write_pin(Pin_Reset);
		XDelayMs(100);
		led.write_pin(Pin_Set);
		XDelayMs(100);
	}
	return 0;
}
