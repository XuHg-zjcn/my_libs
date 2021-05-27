/*
 * sender.hpp
 *
 *  Created on: 2021年5月11日
 *      Author: xrj
 */

#ifndef DATAFLOW_INC_SENDER_HPP_
#define DATAFLOW_INC_SENDER_HPP_

#include "buffer.hpp"
#include "mylibs_config.hpp"
#ifdef USE_USB
#include "usbd_customhid.h"
#endif
#include "ssd1306.hpp"


#ifdef USE_USB
typedef struct{
	Buffer *buf;
	USBD_HandleTypeDef *pdev;
	u32 n;
}SenderParam;
#endif

/*
 * Show_fos only for `OLEDShow_ms_TimerFunc`,
 * use `Interval_xx_Frames` in ssd1306.hpp as `ShowParam.fos_N_ms`
 */
typedef enum{
	Show_Scroll,
	Show_Fixed,
	Show_fos
}ShowType;

typedef struct{
	Buffer *buf;
	SSD1306 *oled;
	ShowType type;
	uint32_t N_ms_fos;
	uint8_t y0; //used for, 暂时不支持
	uint8_t y1; //oled->commd_bytes(SET_PAGE_ADDR_2B, y0, y1);
}ShowParam;

#ifdef USE_FREERTOS
#ifdef USE_USB
void UsbSender_TaskFunc(void *argument);
#endif
void OLEDShow_N_TaskFunc(void *argument);
void OLEDShow_ms_TimerFunc(void *argument);
#endif

#endif /* DATAFLOW_INC_SENDER_HPP_ */
