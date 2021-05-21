/*
 * ssd1306.cpp
 *
 *  Created on: May 12, 2021
 *      Author: xrj
 */

#include <ssd1306.hpp>
#include <cstdarg>
#include <cstring>
#include "ops.h"
#include "FreeRTOS.h"

/*
 * coding rules:
 * use blocking transmit or receive for commands
 */


SSD1306::SSD1306(C_I2C_Dev *dev)
{
	this->dev = dev;
	col_i = 0xff;
}

void SSD1306::commd_bytes(uint8_t Byte0, ...)
{
	int size;
	uint8_t bytes[8];
	switch(Byte0){
	case 0x00 ... 0x1F:   //Set Lower/Higher Column Start Address for Page Addressing Mode
	case 0x40 ... 0x7F:   //Set Display Start Line
	case 0xA4 ... 0xA7:   //Entire display ON, Set Normal/Inverse Display
	case 0xB0 ... 0xB7:   //Set Page Start Address for Page Addressing Mode
	case 0x2E: case 0x2F: //Deactivate/Activate scroll
	case 0xA0: case 0xA1: //Set Segment Re-map
	case 0xAE: case 0xAF: //Set Display ON/OFF
	case 0xC0: case 0xC8: //Set COM Output Scan Direction
	case 0xE3:            //NOP
		size = 1;
		break;
	case 0x20:  //Set Memory Addressing Mode
	case 0x81:  //Set Contrast Control
	case 0xA8:  //Set Multiplex Ratio
	case 0xD3:  //Set Display Offset
	case 0xD5:  //Set Display Clock Divide Ratio/ Oscillator Frequency
	case 0xD9:  //Set Pre-charge Period
	case 0xDA:  //Set COM Pins Hardware Configuration
	case 0xDB:  //Set V COMH Deselect Level
	case 0x8D:  //change pump
		size = 2;
		break;
	case 0xA3:  //Set Vertical Scroll Area
	case 0x21:  //Set Column Address
	case 0x22:  //Set Page Address
		size = 3;
		break;
	case 0x26: case 0x27:
		size = 7;
		break;
	case 0x29: case 0x2A:
		size = 8;
		break;
	default:
		return;
	}
	bytes[0] = Byte0;
    va_list list;
    va_start(list, Byte0);
    for(int i=1;i<size;i++){
    	bytes[i] = va_arg(list, int);
    }
    va_end(list);
    dev->Mem_write(ConByte_Cmd, bytes, size);
}

void SSD1306::ScrollSetup(ScrollSetupCommd* commd)
{
	uint16_t size;
	commd->dummp = 0x00;
	switch(commd->type){
	case Scroll_Right: case Scroll_Left:
		size = 7;
		commd->v_offset = 0x00;
		commd->dummp_ff = 0xff;
		break;
	case Scroll_VertRight: case Scroll_VertLeft:
		size = 6;
		break;
	default:
		return;
	}
	dev->Mem_write(ConByte_Cmd, (uint8_t*)commd, size);
}

void SSD1306::OnOffScroll(bool IsActivate)
{
	uint8_t commd = IsActivate ? ACTIVATE_SCROLL : DEACTIVATE_SCROLL;
	commd_bytes(commd);
}

void SSD1306::Init()
{
	  HAL_Delay(500);
	  commd_bytes(DISPLAY_OFF);
	  commd_bytes(LOW_COL_ADDR_4b  | 0b0000);
	  commd_bytes(HIGH_COL_ADDR_4b | 0b0000);
	  commd_bytes(START_LINE_6b | 0b000000);
	  commd_bytes(CONTRAST_1B, 0x1F);  //亮度
	  commd_bytes(SEG_REMAP1);
	  commd_bytes(NORMAL_DISPLAY);
	  commd_bytes(MULTIPLEX_RATIO_1B,  0x3F);
	  commd_bytes(DISPLAY_OFFSET_1B,   0x00);
	  commd_bytes(CLOCK_SETTING_1B,    0xF0);
	  commd_bytes(PRECHARGE_PERIOD_1B, 0xF1);
	  commd_bytes(COM_HW_PINS_CONF_1B, 0x12);
	  commd_bytes(V_COMH_DESELECT_1B,  0x40);
	  commd_bytes(CHARGE_PUMP_SET_1B,  0x14);  //电荷泵

	  commd_bytes(ADDRESSING_MODE_1B,  0x01);
	  /*commd_bytes(START_PAGE_ADDR_4b | 0b0000);
	  commd_bytes(OUTSCAN_INV);*/
	  commd_bytes(DISPLAY_FOLLOWS_RAM);
	  commd_bytes(DISPLAY_ON);
}

void SSD1306::fill(uint8_t data)
{
	uint8_t* x128 = (uint8_t*)pvPortMalloc(1024);
	memset(x128, data, 1024);
	commd_bytes(ADDRESSING_MODE_1B,  0x01);
	commd_bytes(SET_COLUMN_ADDR_2B, 0, 127);  //page0-page1
	commd_bytes(SET_PAGE_ADDR_2B, 0, 7);  //page0-page1
	dev->Mem_write(ConByte_Data, x128, 1024);
	vPortFree(x128);
}

void SSD1306::plot_128(uint8_t *data, uint8_t bias, uint8_t maxh)
{
	uint64_t col;
	uint32_t data2;
	commd_bytes(ADDRESSING_MODE_1B,  0x01);
	commd_bytes(SET_COLUMN_ADDR_2B, 0, 127);  //page0-page1
	commd_bytes(SET_PAGE_ADDR_2B, 0, 7);  //page0-page1
	for(int i=0;i<128;i++){
		data2 = value_upper(*data, maxh) + bias;
		col = 1ULL<<data2;  //ULL = uint64_t
		dev->Mem_write(ConByte_Data, (uint8_t*)&col, 8);
		data++;
	}
}

void SSD1306::VH_scroll(int dx, int dy, uint8_t sta_page, uint8_t end_page, FrameOfStep frames)
{
	ScrollSetupCommd commd;
	switch(dx){
	case -1:
		col_i = 127;
		break;
	case 1:
		col_i = 0;
		break;
	default:
		return;
	}
	if(dy!=0){
		commd.type = dx==-1 ? Scroll_VertLeft : Scroll_VertRight;
	}else{
		commd.type = dx==-1 ? Scroll_Left : Scroll_Right;
	}
	commd.sta_page = sta_page;
	commd.frames = frames;
	commd.end_page = end_page;
	commd.v_offset = dy<0 ? 63+dy : dy;
	ScrollSetup(&commd);
	commd_bytes(ACTIVATE_SCROLL);
	commd_bytes(SET_COLUMN_ADDR_2B, col_i, col_i+1);  //page0-page1
}

void SSD1306::Scroll_Disable()
{
	commd_bytes(DEACTIVATE_SCROLL);
	col_i = 0xff;
}

void SSD1306::append_column(uint64_t col)
{
	if(col_i != 0xff){  //滚动模式
		//减少传输错误影响，避免大面积混乱
		commd_bytes(SET_COLUMN_ADDR_2B, col_i, col_i+1);
	}
	dev->Mem_write(ConByte_Data, (uint8_t*)&col, 8);
}

void SSD1306::gif_show(uint8_t *imgs, uint32_t n_img, uint32_t ms)
{
	commd_bytes(ADDRESSING_MODE_1B,  0x01);
	commd_bytes(SET_COLUMN_ADDR_2B, 0, 127);
	commd_bytes(SET_PAGE_ADDR_2B, 0, 7);
	for(uint32_t i=0;i<n_img;i++){
		dev->Mem_write(ConByte_Data, imgs+i*1024, 1024);
		HAL_Delay(ms);
	}
}

//void SSD1306::print()
