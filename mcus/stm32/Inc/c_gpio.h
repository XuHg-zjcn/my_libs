/*
 * my_gpio.h
 *
 *  Created on: 2021年4月16日
 *      Author: xrj
 */

#ifndef C_GPIO_H_
#define C_GPIO_H_

#include "bit_band.h"

//advice use class `C_Pin` to get IDR or ODR bitband, create object from bitband address.

//checks
#define IS_PIN_N(PIN_N)                      (0<=PIN_N<16)

//convert to bit-band pointer
#define BITBAND_GPIO_ODR_N(GPIOx, Pin_N)     BIT_PTR(&(GPIOx->ODR), Pin_N)
#define BITBAND_GPIO_ODR_2N(GPIOx, GPIO_Pin) BIT_PTR(&(GPIOx->ODR), __builtin_ctz(GPIO_Pin))
#define BITBAND_GPIO_IDR_N(GPIOx, Pin_N)     BIT_PTR(&(GPIOx->IDR), Pin_N)
#define BITBAND_GPIO_IDR_2N(GPIOx, GPIO_Pin) BIT_PTR(&(GPIOx->IDR), __builtin_ctz(GPIO_Pin))

//get GPIOx, GPIO_Pin from bit-band pointer
#define BITBAND_GPIO_GPIOx(band)             ((GPIO_TypeDef*)(BITBAND_SRCADDR(band)&0xfffffc00))
#define BITBAND_GPIO_PIN_N(band)             (((uint32_t)band/4)&0xf)
#define BITBAND_GPIO_PIN_2N(band)            (1<<BITBAND_GPIO_PIN_N(band))

#endif /* MY_GPIO_H_ */
