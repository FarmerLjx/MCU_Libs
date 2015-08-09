/* 
* @FileName: ad9954.h
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:14
* @Description: AD9954驱动头文件，包含引脚定义，AD9954寄存器地址定义，以及函数声明
* @Modified by  |  Modified time  |  Description 
* 
*/

#ifndef __AD9954_H__
#define __AD9954_H__

// Macro for AD9954 -- I/O
#define AD9954_CLK_H   (P8OUT |= BIT1)    // P8.1 -- SCLK
#define AD9954_CLK_L   (P8OUT &= ~BIT1)
#define AD9954_SDIO_H  (P8OUT |= BIT2)    // P8.2 -- SDIO
#define AD9954_SDIO_L  (P8OUT &= ~BIT2)
#define AD9954_CS_H    (P8OUT |= BIT4)    // P8.4 -- CSB
#define AD9954_CS_L    (P8OUT &= ~BIT4)
#define IO_UPDATE_H    (P9OUT |= BIT2)    // P9.2 -- UPDATE
#define IO_UPDATE_L    (P9OUT &= ~BIT2)
#define AD9954_PS1_H   (P9OUT |= BIT3)    // P9.3 -- PS1
#define AD9954_PS1_L   (P9OUT &= ~BIT3)
#define AD9954_PS0_H   (P9OUT |= BIT4)    // P9.4 -- PS0
#define AD9954_PS0_L   (P9OUT &= ~BIT4)
#define AD9954_OSK_H   (P9OUT |= BIT5)    // P9.5 -- OSK
#define AD9954_OSK_L   (P9OUT &= ~BIT5)

// Freq factor
#define AD9954_FREQ_PER_KHZ  (10737.418)  // per kHz
#define AD9954_FREQ_PER_HZ   (10.737418)  // per Hz

// Defines of AD9954's Register
#define 	CFR1		0x00
#define 	CFR2 		0x01
#define 	ASF   		0x02
#define 	ARR 		0x03
#define 	FTW0		0x04
#define 	POW0		0x05
#define 	FTW1		0x06
#define 	NLSCW		0x07
#define 	PLSCW		0x08
#define 	RSCW0 		0x07
#define 	RSCW1 		0x08
#define 	RSCW2 		0x09
#define 	RSCW3 		0x0a
#define 	RAM 		0x0b

// Function list
void AD9954Init(void);
void AD9954Send(unsigned char dat);
void SetFreqToHz(unsigned long ulFreq);
void AD9954SetReg8(unsigned char ucRegName, unsigned int uiValue);
void LinearSweepMode(void);
void RAMControlledMode(void);
void RamInit4PS(void);
void RamInit4FS(void);
void PhaseShift(unsigned int uiValue);
void SingleTone(void);

#endif    // !__AD9954_H__

