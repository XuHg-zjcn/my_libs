/*
 * bit_band.h
 *
 *  Created on: Apr 16, 2021
 *      Author: xrj
 */

#ifndef INC_BIT_BAND_H_
#define INC_BIT_BAND_H_

#define BITBAND(addr, bitnum)  ((((addr)&(~0xffffff))|0x2000000) + (((addr)&0xffffff)*8+(bitnum))*4)
#define BIT_PTR(addr, bitnum)  ((uint32_t*)BITBAND((uint32_t)(addr), bitnum))
#define BIT_VAL(addr, bitnum)  (*BIT_PTR(addr, bitnum))
#define BITBAND_SRCADDR(band)  (((uint32_t)band&(~0xfffffff)) + (((uint32_t)band&0x1ffffff)/32))
#define BITBAND_SRCNUM(band)   (((uint32_t)band/4)&0x7)

#endif /* INC_BIT_BAND_H_ */
