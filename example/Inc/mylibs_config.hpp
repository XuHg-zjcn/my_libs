#ifndef CONFIG_H
#define CONFIG_H

#define USE_ABSTRACT
#define USE_DMA
#define USE_USB
#define USE_FREERTOS

#include "stm32f1xx_hal.h"
#include "f1_gpio.hpp"

#ifdef USE_FREERTOS
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#define XMalloc pvPortMalloc
#define XFree vPortFree
#else
#include <stdlib.h>
#define XMalloc malloc
#define XFree free
#endif

#ifdef USE_USB
#include "usbd_customhid.h"
#endif

#endif
