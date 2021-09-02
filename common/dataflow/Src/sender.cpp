/*
 * usb_sender.cpp
 *
 *  Created on: 2021年5月11日
 *      Author: xrj
 */

#include "sender.hpp"
#include "buffhead.hpp"
#include "main.h"


/*
 * example code, will use each below:
 *
 * extern USBD_HandleTypeDef hUsbDeviceFS;
 * extern ADC_HandleTypeDef hadc1;
 * extern TIM_HandleTypeDef htim2;
 * C_ADCEx adc = C_ADCEx(&hadc1, &htim2);
 * Buffer buff = Buffer(2);
 *
 * buff.Init();
 * buff.remalloc(128);
 * adc.set_SR_sps(10000);
 * adc.conn_buff(&buff.w_head);
 */

#ifdef USE_FREERTOS
#ifdef USE_USB
/*
 * send buffer data via usb
 * example:
 * osThreadAttr_t attr={.name="usb_sender"};
 * SenderParam param = {.buf=buff, .pdev=&hUsbDeviceFS, 32}; //2*32=64 Byte per pack max
 * osThreadNew(&UsbSender_TaskFunc, &param, &attr);
 * adc.DMA_cycle(100);
 */
void UsbSender_TaskFunc(void *argument)
{
	SenderParam *param = (SenderParam*)argument;
	BuffHeadRead head = BuffHeadRead(*param->r_heads, param->r_heads->new_head());
	u8* p;
	u32 be = head.bytes_elem();
	while(1){
		p = (u8*)head.get_frames(param->n);
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		while(USBD_CUSTOM_HID_SendReport(param->pdev, p, param->n*be)!=USBD_OK);
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	}
}
#endif

void OLEDShow_config(ShowParam *param)
{
	param->oled->commd_bytes(Addressing_Mode_1B,  Vert_Mode);
	param->oled->commd_bytes(V_Scorll_Area_2B, param->y0, param->y1);
}

void OLEDShow_N_TaskFunc(void *argument)
{
	ShowParam *param = (ShowParam*)argument;
	OLEDShow_config(param);
	uint32_t head_i = param->r_heads->new_head();
	uint16_t *p;
	uint32_t n = param->N_merge;
	uint64_t column;
	switch(param->type){
	    case Show_Fixed:
		    for(;;){
			    p = (uint16_t*)param->r_heads->get_frames(head_i, n*128);
			    if(!p){
					osThreadExit();
			    }
			    param->oled->commd_bytes(Addressing_Mode_1B,  Vert_Mode);
			    param->oled->commd_bytes(Set_Column_Addr_2B, 0, 127);
			    param->oled->commd_bytes(Set_Page_Addr_2B, 0, 7);
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

void OLEDShow_TimerFunc(void *argument)
{
	ShowParam *param = (ShowParam*)argument;
}
#endif
