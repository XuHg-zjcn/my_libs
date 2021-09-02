#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp8266/rom_functions.h"

#define XDelayUs(x) ets_delay_us(x)
#define XDelayMs(x) vTaskDelay(x)
