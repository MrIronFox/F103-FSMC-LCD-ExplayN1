/*
 * lcd_explay_n1.h
 *
 *  Created on: 30 окт. 2020 г.
 *      Author: Home_PC
 */

#ifndef INC_LCD_EXPLAY_N1_H_
#define INC_LCD_EXPLAY_N1_H_

#include "main.h"
#include "stdbool.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

#define swap(type, i, j) {type t = i; i = j; j = t;}
#define pgm_read_word(data) *data
#define pgm_read_byte(data) *data
#define fontbyte(x) cfont.font[x]


#define VGA_BLACK    	0x0000
#define VGA_WHITE   	0xFFFF
#define VGA_RED     	0xF800
#define VGA_GREEN   	0x0400
#define VGA_BLUE    	0x001F
#define VGA_SILVER    	0xC618
#define VGA_GRAY    	0x8410
#define VGA_MAROON    	0x8000
#define VGA_YELLOW    	0xFFE0
#define VGA_OLIVE   	0x8400
#define VGA_LIME    	0x07E0
#define VGA_AQUA    	0x07FF
#define VGA_TEAL    	0x0410
#define VGA_NAVY    	0x0010
#define VGA_FUCHSIA   	0xF81F
#define VGA_PURPLE    	0x8010
#define VGA_TRANSPARENT 0xFFFFFFFF

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

#define PORTRAIT 0
#define LANDSCAPE 1

#define ADDR_CMD        (*(uint8_t *) 0x60000000)
#define ADDR_DATA       (*(uint8_t *) 0x60020000)
#define ADDR_DATA16     (*(uint16_t *) 0x60020000)

typedef struct
{
	uint8_t* font;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
} _current_font;

uint16_t f_color;
uint16_t b_color;
bool _transparent;
_current_font cfont;

void LCD_Delay(uint32_t dly);
void DelayMicro(uint32_t micros);
void SetXY(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void clrXY();
void LCD_fillScr(uint16_t color);
void LCD_clrScr();
uint32_t LCD_Read(uint16_t RG);
void setColorRGB(uint8_t r, uint8_t g, uint8_t b);
void setColor(uint16_t color);
uint16_t getColor();
void setBackColorRGB(uint8_t r, uint8_t g, uint8_t b);
void fillScr(uint16_t color);
void fillScrRGB(uint8_t r, uint8_t g, uint8_t b);
void setBackColor(uint32_t color);
uint16_t getBackColor();
int getDisplayXSize();
int getDisplayYSize();
void setPixel(uint16_t color);
void drawPixel(int x, int y);
void drawHLine(int x, int y, int l);
void drawVLine(int x, int y, int l);
void drawLine(int x1, int y1, int x2, int y2);
void drawRect(int x1, int y1, int x2, int y2);
void drawRoundRect(int x1, int y1, int x2, int y2);
void fillRect(int x1, int y1, int x2, int y2);
void fillRoundRect(int x1, int y1, int x2, int y2);
void drawCircle(int x, int y, int radius);
void fillCircle(int x, int y, int radius);
void printChar(uint8_t c, int x, int y);
void rotateChar(uint8_t c, int x, int y, int pos, int deg);
void print(char *st, int x, int y, int deg);
void printNumI(long num, int x, int y, int length, char filler);
void _convert_float(char *buf, double num, int width, uint8_t prec);
void printNumF(double num, uint8_t dec, int x, int y, char divider, int length, char filler);
void setFont(uint8_t* font);
uint8_t* getFont();
uint8_t getFontXsize();
uint8_t getFontYsize();
void InitLCD();




#endif /* INC_LCD_EXPLAY_N1_H_ */
