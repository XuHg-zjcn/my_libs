#include "app.h"
#include "delay.hpp"
#include "c_pin.hpp"
#include "s_i2c.hpp"
#include "ssd1306.hpp"
#include "c_adc.hpp"
#include "ops.hpp"
char str[20];

int app(void)
{
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE);

	C_ADC *cadc = ((C_ADC *)ADC1);
	cadc->Init();

	C_Pin scl = C_Pin(1, 10);
	C_Pin sda = C_Pin(1, 11);
	scl.loadXCfg(GPIO_GP_OD1);
	sda.loadXCfg(GPIO_GP_OD1);
	S_I2C si2c = S_I2C(scl, sda);
	si2c.set_clock(100000);

	S_I2C_Dev dev = S_I2C_Dev(&si2c, Addr_OLED);
	SSD1306 oled = SSD1306(&dev);
	oled.Init();
	oled.fill(0x00);
	oled.VH_scroll(-1, 0, 0, 7, Interval_05_Frames);
	while(1){
		for(u16 i=0;i<256;i++){
			u16 tmp = cadc->read_channel(CADC_CH0, CADC_tSMP_28Cyc5);
			oled.append_column(U64_BOTTOM>>(tmp/64));
			XDelayMs(50);
		}
	}
	return 0;
}
