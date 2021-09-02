#include "c_pin.hpp"
#include "driver/gpio.h"


#define GPIO_NUM_X(x) ((gpio_num_t)x)

C_Pin::C_Pin(u8 pin_no)
{
    this->pin_no = pin_no;
}

void C_Pin::write_pin(PinState x)
{
    gpio_set_level(GPIO_NUM_X(pin_no), x);
}

PinState C_Pin::read_pin()
{
    return (PinState)gpio_get_level(GPIO_NUM_X(pin_no));
}

void C_Pin::loadCfg(PinCfg cfg)
{
    gpio_config_t conf;
    conf.pin_bit_mask = 1ULL<<pin_no;
    conf.pull_up_en = GPIO_PULLUP_DISABLE;
    conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    switch(cfg){
        case Pin_PP0: case Pin_PP1:
            conf.mode = GPIO_MODE_OUTPUT;
            break;
        case Pin_OD0: case Pin_OD1:
            conf.mode = GPIO_MODE_OUTPUT_OD;
            break;
        case Pin_InUp:
            conf.mode = GPIO_MODE_INPUT;
            conf.pull_up_en = GPIO_PULLUP_ENABLE;
            break;
        case Pin_InFlt:
            conf.mode = GPIO_MODE_INPUT;
            break;
        case Pin_InDown:
            conf.mode = GPIO_MODE_INPUT;
            conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
            break;
    }
    gpio_config(&conf);
    if(PIN_IS_OUT(cfg)){
         gpio_set_level(GPIO_NUM_X(pin_no), cfg&0x01);
    }
}

void C_Pin::toggle_pin()
{
	write_pin(!read_pin());
}

//blocking until read_pin() == state
void C_Pin::wait_pin(PinState state)
{
	while(read_pin() xor state);
}

u32 C_Pin::wait_timeout(PinState state, u32 timeout)
{
	while((read_pin() xor state) and timeout){
		timeout--;
	}
	return timeout;
}

//阻塞式测量
u32 C_Pin::wait_count(PinState state, u32 m, u32 M)
{
	u32 n=0;
	state = !state;
	while(((read_pin() xor state) and n<M) or n<m){
		n++;
	}
	return n;
}

/*void C_Pin::set_EXTI_type(EXTI_Type type)
{
    gpio_set_intr_type((gpio_num_t)pin_no, (gpio_int_type_t)type);
}

void C_Pin::set_EXTI_isr(void (*fn)(void *), void *arg)
{
    gpio_isr_register(fn, arg, ESP_INTR_FLAG_LEVEL4, nullptr);
}

void C_Pin::EXTI_enable()
{
    gpio_intr_enable((gpio_num_t)pin_no);
}

void C_Pin::EXTI_disable()
{
    gpio_intr_disable((gpio_num_t)pin_no);
}*/
