#define USE_LCD 1

#include "app.h"
#include "delay.hpp"
#include "c_pin.hpp"
#include "s_i2c.hpp"
#include "c_tim.hpp"

#if USE_LCD
#include "ssd1306.hpp"

char str[20];
#endif

int app(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	C_Pin pwm = C_Pin(1, 9);
	pwm.loadXCfg(GPIO_AF_PP & Out_2MHz);
	C_TIM *ctim = (C_TIM *)TIM4;
	ctim->PWM_Init();
	ctim->set_autoload(1000);
	ctim->set_comp(CTIM_Channel_4, 0);
	ctim->CCxChannelPolar(CTIM_Channel_4, Pin_Set);
	ctim->PWM_Start(CTIM_Channel_4);

#if USE_LCD
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
#endif
	while(1){
		for(int i=0;i<1000;i++){
#if USE_LCD
			snprintf(str, 20, "%03d", i);
			oled.setVHAddr(Vert_Mode, 0, 127, 0, 0);
			oled.text_5x7(str);
#endif
			ctim->set_comp(CTIM_Channel_4, i);
			XDelayMs(10);
		}
	}
	return 0;
}
