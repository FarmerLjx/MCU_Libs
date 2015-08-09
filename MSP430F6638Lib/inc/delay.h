/* 
* @FileName: delay.h
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:15
* @Description: 延时头文件
* @Modified by  |  Modified time  |  Description 
*  
*/


#ifndef DELAY_H_
#define DELAY_H_

#include <stdint.h>

#define delay_40ns __no_operation();	// delay 40 ns
void delay_ms(unsigned int n);
void delay_us(unsigned int n);

extern void delay_1ms(unsigned int nValue);    //delay (nValue) ms
extern void delay_140us(unsigned int nValue);  //delay (nValue * 140)us
extern void delay_1us(unsigned int nValue);    //delay (0.88 + 0.16 * nValue)us


#endif /* DELAY_H_ */
