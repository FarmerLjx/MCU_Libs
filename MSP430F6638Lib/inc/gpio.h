/* 
* @FileName: gpio.h
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:15
* @Description: GPIO引脚结构体声明，目的是更方便使用I/O读写
* @Modified by  |  Modified time  |  Description 
*  
*/


#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>

typedef struct __WORD_S {
	uint16_t bit0    : 1 ;
	uint16_t bit1    : 1 ;
	uint16_t bit2    : 1 ;
	uint16_t bit3    : 1 ;
	uint16_t bit4    : 1 ;
	uint16_t bit5    : 1 ;
	uint16_t bit6    : 1 ;
	uint16_t bit7    : 1 ;
}WordS;

union GPIO_REG {
	uint8_t wReg;
	WordS wBit;
};

// Port P8
extern  union GPIO_REG *  P8INREG;
extern  union GPIO_REG *  P8OUTREG;
extern  union GPIO_REG *  P8DIRREG;

// Port P9
extern  union GPIO_REG *  P9INREG;
extern  union GPIO_REG *  P9OUTREG;
extern  union GPIO_REG *  P9DIRREG;

// Add more here if need

#endif // ! __GPIO_H__
