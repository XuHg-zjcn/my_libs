#include "app.h"
#include "delay.hpp"
#include "c_pin.hpp"
#include "s_i2c.hpp"
#include "ssd1306.hpp"

char str[20];

int app(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	C_Pin scl = C_Pin(1, 10);
	C_Pin sda = C_Pin(1, 11);
	scl.loadXCfg(GPIO_AF_OD);
	sda.loadXCfg(GPIO_AF_OD);
	S_I2C si2c = S_I2C(scl, sda);
	si2c.set_clock(100000);
	S_I2C_Dev dev = S_I2C_Dev(&si2c, Addr_OLED);
	SSD1306 oled = SSD1306(&dev);
	oled.Init();
	oled.fill(0x00);
	while(1){
		for(int i=0;i<1000;i++){
			snprintf(str, 20, "%03d", i);
			oled.setVHAddr(Vert_Mode, 0, 127, 0, 0);
			oled.text_5x7(str);
			oled.setVHAddr(Vert_Mode, 3, 127, 1, 0);
			oled.text_3x5(str, 3);
			XDelayMs(100);
		}
	}
	return 0;
}
