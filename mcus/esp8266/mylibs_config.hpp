#ifndef MYLIBS_CONFIG_HPP
#define MYLIBS_CONFIG_HPP

//the config is for rtos sdk

#include "esp8266/Inc/mylibs_esp8266.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define XMalloc(n)  pvPortMalloc(n)
#define XFree(p)    vPortFree(p)
#define XDelayMs(x) vTaskDelay(x)

#endif
