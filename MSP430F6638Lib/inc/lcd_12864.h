#ifndef LCD_12864_H
#define LCD_12864_H

#include <msp430.h>
#include "delay.h"

#define uchar unsigned char
#define uint unsigned int

// LCD Pin 7  : SCLK  --  P5.3
// LCD Pin 8  : DI    --  P5.4
// LCD Pin 9  : VCC   --  3.3V / 5.0V
// LCD Pin 10 : GND   --  GND
// LCD Pin 11 : LEDA  --  NC
// LCD Pin 13 : CS    --  P5.5
// LCD Pin 13 : RES   --  P5.6
// LCD Pin 14 : A0    --  P7.4

#define	 SCLK	BIT3 //P5.3
#define	 DI		BIT4 //P5.4
#define	 CS	    BIT5 //P5.5
#define	 RST	BIT6 //P5.6
#define	 A0		BIT4 //P7.4

#define  SCLK_DIR 	P5DIR
#define  DI_DIR  	P5DIR
#define  CS_DIR   	P5DIR
#define  RST_DIR  	P5DIR
#define  A0_DIR  	P7DIR

#define  SCLK_OUT	P5OUT
#define  DI_OUT		P5OUT
#define  CS_OUT		P5OUT
#define  RST_OUT	P5OUT
#define  A0_OUT		P7OUT

/*************Interface Define*****************/
#define SET_SCL()		SCLK_DIR |= SCLK;	SCLK_OUT |= SCLK
#define CLR_SCL()		SCLK_DIR |= SCLK;	SCLK_OUT &= ~SCLK
#define SET_DI()		DI_DIR |= DI;		DI_OUT |= DI
#define CLR_DI()		DI_DIR |= DI;		DI_OUT &= ~DI
#define SET_CS()		CS_DIR |= CS;		CS_OUT |= CS
#define CLR_CS()		CS_DIR |= CS;		CS_OUT &= ~CS
#define SET_RST()		RST_DIR |= RST;		RST_OUT |= RST
#define CLR_RST()		RST_DIR |= RST;		RST_OUT &= ~RST
#define SET_A0()		A0_DIR |= A0;		A0_OUT |= A0
#define CLR_A0()		A0_DIR |= A0;		A0_OUT &= ~A0

// new added on 2015/08/06
void lcd_pixelDraw(unsigned char x,unsigned char y,unsigned char color);
void lcd_setAddress(unsigned char row,unsigned char col);
void lcd_circleDraw(unsigned char x,unsigned char y,unsigned char radius ,unsigned char color);
void lcd_lineDraw(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color);

void data_send(unsigned char dat);
void wr_com(unsigned char com);
void wr_data(unsigned char dat);
void display_rect();
void display_lattice(unsigned char dat1,unsigned char dat2);
void display_map(unsigned char *p);
void set_page_address(uchar page);
void set_colume_address(uchar colume);
void disp_word(uchar row,uchar col,uchar ascii);
void disp_chn(uchar row, uchar col, uchar cn);
void LcdClear();
void LcdInit();
void disp_str(uchar row,uchar col,uchar *cn);
void DisplayByteInHex(unsigned char value, uchar row, uchar col);
int  DisplayIntInHex(unsigned int value, uchar row, uchar col);
int  DisplayULongInDec(unsigned long value, unsigned char row, unsigned char col);
int DisplayDoubleInDec(double value, int n, unsigned char row, unsigned char col);

#endif
