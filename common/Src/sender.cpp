/*
 * usb_sender.cpp
 *
 *  Created on: 2021年5月11日
 *      Author: xrj
 */

#include "sender.hpp"
#include "buffhead.hpp"
#include "main.h"

void UsbSender_TaskFunc(void *argument)
{
	SenderParam *param = (SenderParam*)argument;
	BuffHeadReads *r_heads = &param->buf->r_heads;
	BuffHeadRead head = BuffHeadRead(*r_heads, r_heads->new_head());
	u8* p;
	u32 be = head.bytes_elem();
	while(1){
		p = (u8*)head.get_frames(param->n);
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		USBD_CUSTOM_HID_SendReport(param->pdev, p, param->n*be);
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	}
}

void OLEDShow_config(ShowParam *param)
{
	param->oled->commd_bytes(ADDRESSING_MODE_1B,  0x01);
	param->oled->commd_bytes(V_SCORLL_AREA_2B, param->y0, param->y1);
}

void OLEDShow_N_TaskFunc(void *argument)
{
	ShowParam *param = (ShowParam*)argument;
	OLEDShow_config(param);
	uint32_t head_i = param->buf->r_heads.new_head();
	uint16_t *p;
	uint32_t n = param->N_ms_fos;
	uint64_t column;
	switch(param->type){
	    case Show_Fixed:
		    for(;;){
			    p = (uint16_t*)param->buf->r_heads.get_frames(head_i, n*128);
			    if(!p){
					osThreadExit();
			    }
			    param->oled->commd_bytes(ADDRESSING_MODE_1B,  0x01);
			    param->oled->commd_bytes(SET_COLUMN_ADDR_2B, 0, 127);
			    param->oled->commd_bytes(SET_PAGE_ADDR_2B, 0, 7);
			    for(uint32_t i=0;i<128;i++){
					column = 0;
					for(uint32_t j=0;j<n;j++){
						column |= U64_BOTTOM>>(*p++/64);
					}
					param->oled->append_column(column);
				}
		    }
			break;
		case Show_Scroll:
			break;//暂时没有实现
	    case Show_fos:
			break;//禁止使用
	    default:
			break;
	}
}

void OLEDShow_ms_TimerFunc(void *argument)
{
	ShowParam *param = (ShowParam*)argument;
	OLEDShow_config(param);
}
