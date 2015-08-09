/* 
* @FileName: HAL_Board.h
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:15
* @Description: 
* @Modified by  |  Modified time  |  Description 
*     PeeNut        2015-08-09       移除stdint.h头文件，uint8_t用unsigned int代替
*/

#ifndef __HAL_BOARD_H__
#define __HAL_BOARD_H__

#include <msp430.h>

#define XT2_PORT_DIR            P7DIR
#define XT2_PORT_OUT            P7OUT
#define XT2_PORT_SEL            P7SEL
#define XT2_ENABLE              (BIT2 + BIT3)

#define LED12_PORT_DIR          P9DIR
#define LED12_PORT_OUT          P9OUT
#define LED1                    (0x01)
#define LED2                    (0x02)
#define LED_ALL                 (0x03)

extern void Board_init(void);
extern void Board_ledOn(unsigned int ledMask);
extern void Board_ledOff(unsigned int ledMask);
extern void Board_ledToggle(unsigned int ledMask);

#endif /* __HAL_BOARD_H__ */
