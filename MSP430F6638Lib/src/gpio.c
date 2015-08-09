/* 
* @FileName: gpio.c
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:15
* @Description: MSP430F6638的GPIO，定义为I/O的寄存器地址为一个联合结构体，方便I/O读写和控制
* @Modified by  |  Modified time  |  Description 
*  
*/

#include <msp430.h>
#include "../inc/gpio.h"

// Port define
// Port P8
union GPIO_REG *  P8INREG   = (union GPIO_REG *)0x0261;
union GPIO_REG *  P8OUTREG  = (union GPIO_REG *)0x0263;
union GPIO_REG *  P8DIRREG  = (union GPIO_REG *)0x0265;

// Port P9
union GPIO_REG *  P9INREG   = (union GPIO_REG *)0x0280;
union GPIO_REG *  P9OUTREG  = (union GPIO_REG *)0x0282;
union GPIO_REG *  P9DIRREG  = (union GPIO_REG *)0x0284;


// GPIO distribution
