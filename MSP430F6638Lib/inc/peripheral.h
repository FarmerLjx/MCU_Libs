/* 
* @FileName: peripheral.h
* @Author  : PeeNut
* @Date    : 2015-08-08 20:43:58
* @Description: MSP430外设初始化相关，仅提供一个模板，放到实际代码中按实际需求修改
* @Modified by  |  Modified time  |  Description 
*  
*/

#ifndef __PERIPHERAL_H__
#define __PERIPHERAL_H__

#include <msp430.h>

void ADCInit(void);
void CompInit(void);
void UARTInit(void);
void Port_Mapping(void);

#endif  //!__PERIPHERAL_H__
