/*
 * ssd1306.hpp
 *
 *  Created on: May 12, 2021
 *      Author: xrj
 */

#ifndef INC_SSD1306_HPP_
#define INC_SSD1306_HPP_

#include "cmsis_os2.h"
#include "c_i2c.hpp"

/* I2C slave address of SSD1306
 *   MSB  .   .   .   .   .   .  LSB
 * bit7   6   5   4   3   2   1   0
 *  | 0 | 1 | 1 | 1 | 1 | 0 |SA0|R/W|
 */
#define Addr_OLED   0x78  // 0b01111000
#define SA0         0     // State of D/C Pin
#define Write       0
#define Read        1
#define Addr_Write  (Addr_OLED | SA0 | Write)
#define Addr_Read   (Addr_OLED | SA0 | Read)

/* control byte, in HAL as mem_addr.
 *   MSB  .   .   .   .   .   .  LSB
 * bit7   6   5   4   3   2   1   0
 *  |Co |D/C| 0 | 0 | 0 | 0 | 0 | 0 |
 *
 */
#define ConByte_Contin 0x80
#define ConByte_Data   0x40
#define ConByte_Cmd    0x00

/* Control Byte
 *   MSB  .   .   .   .   .   .  LSB
 * bit7   6   5   4   3   2   1   0
 *  |Co |D/C| 0 | 0 | 0 | 0 | 0 | 0 |
 *
 *  Co  – Continuation bit
 *  D/C – Data / Command Selection bit
 *  SA0 – Slave address bit
 */

//from datasheet 9.1 COMMAND TABLE
// ..._nb : n bits at last    commd_bytes(..._nb | bits);
// ..._nB : n Bytes params    commd_bytes(..._nB, Byte1, Byte2, ..., ByteN);
// ..._nbmB: n bits, m Bytes  commd_bytes(..._nbmB | bits, Byte1, ..., ByteM);

#define DISPLAY_FOLLOWS_RAM   0xA4
#define DISPLAY_IGNORES_RAM   0xA5

#define NORMAL_DISPLAY        0xA6
#define INVERSE_DISPLAY       0xA7

#define DISPLAY_OFF           0xAE
#define DISPLAY_ON            0xAF

#define H_SCORLL_1b6B         0x26 //advice use:
#define VH_SCORLL_2b5B        0x28 //SSD1306::VH_scroll(...);
#define DEACTIVATE_SCROLL     0x2E //SSD1306::Scroll_Disable();
#define ACTIVATE_SCROLL       0x2F
#define V_SCORLL_AREA_2B      0xA3

#define ADDRESSING_MODE_1B    0x20
#define HORZ_MODE             0x00
#define VERT_MODE             0x01
#define PAGE_MODE             0x02

//commands in Horizontal/Vertical Mode
#define SET_COLUMN_ADDR_2B    0x21
#define SET_PAGE_ADDR_2B      0x22

//commands in Page mode
#define LOW_COL_ADDR_4b       0x00  //low 4bit
#define HIGH_COL_ADDR_4b      0x10  //high 4bit
#define START_PAGE_ADDR_4b    0xB0


#define OUTSCAN_NORM          0xC0
#define OUTSCAN_INV           0xC8


#define START_LINE_6b         0x40
#define CONTRAST_1B           0x81

#define SEG_REMAP0            0xA0
#define SEG_REMAP1            0xA1

#define MULTIPLEX_RATIO_1B    0xA8
#define DISPLAY_OFFSET_1B     0xD3

#define CLOCK_SETTING_1B      0xD5
#define PRECHARGE_PERIOD_1B   0xD9
#define COM_HW_PINS_CONF_1B   0xDA
#define V_COMH_DESELECT_1B    0xDB
#define CHARGE_PUMP_SET_1B    0x8D
#define SSD1306_NOP           0xE3

#define U64_TOP            (1ULL)     //use << operate
#define U64_BOTTOM         (1ULL<<63) //use >> operate


typedef enum{
	Scroll_Right = H_SCORLL_1b6B | 0b0,
	Scroll_Left  = H_SCORLL_1b6B | 0b1,
	Scroll_VertRight = VH_SCORLL_2b5B | 0b01,
	Scroll_VertLeft  = VH_SCORLL_2b5B | 0b10
}ScrollType;

typedef enum{
	Interval_05_Frames = 0x0,
	Interval_64_Frames = 0x1,
	Interval_128_Frames = 0x2,
	Interval_256_Frames = 0x3,
	Interval_03_Frames = 0x4,
	Interval_04_Frames = 0x5,
	Interval_25_Frames = 0x6,
	Interval_02_Frames = 0x7,
}FrameOfStep;

#pragma pack(1)
typedef struct{
	ScrollType type:8;    //   | 0 0 1 0 X X X X
	uint8_t dummp;        // A | 0 0 0 0 0 0 0 0
	uint8_t sta_page;     // B | * * * * * B B B
	FrameOfStep frames:8; // C | * * * * * C C C
	uint8_t end_page;     // D | * * * * * D D D
	uint8_t v_offset;     // E | * * E E E E E E
	uint8_t dummp_ff;     // F | 1 1 1 1 1 1 1 1
}ScrollSetupCommd;
#pragma pack()


class SSD1306{
private:
	C_I2C_Dev* dev;
	uint32_t timeout;
	uint8_t col_i;
	TIM_HandleTypeDef* TIM_frame;
	uint8_t *imgs;
	uint32_t n_imgs;
	uint32_t img_i;
#ifdef CMSIS_OS2_H_
	osSemaphoreId_t lock;
#endif
public:
	SSD1306(C_I2C_Dev *dev);
	void commd_bytes(uint8_t Byte0, ...);
	void ScrollSetup(ScrollSetupCommd* commd);
	void OnOffScroll(bool IsActivate);
	void Init();
	void fill(uint8_t data);
	void plot_128(uint8_t *data, uint8_t bias, uint8_t maxh);
	void VH_scroll(int dx, int dy, uint8_t sta_page, uint8_t end_page, FrameOfStep frames);
	void Scroll_Disable();
	void append_column(uint64_t col);
	void gif_show(uint8_t *imgs, uint32_t n_imgs, uint32_t ms);
	void frame_callback();
	void setVHAddr(uint8_t mode, uint8_t col_s, uint8_t col_e, uint8_t page_s, uint8_t page_e);
	void setPageAddr(uint8_t col_s, uint8_t page_s);
	void text_3x5(char* str, uint8_t y);
	void text_5x7(char* str);
};

#endif /* INC_SSD1306_HPP_ */
