/* 
* @FileName: HAL_Board.c
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:15
* @Description: 
* @Modified by  |  Modified time  |  Description 
*     PeeNut        2015-08-09       初始化把所有引脚置零；宏定义移到头文件中
*/

#include "../inc/HAL_Board.h"


/***************************************************************************//**
 * @brief  Initialize the board - configure ports
 * @param  None
 * @return none
 ******************************************************************************/

void Board_init(void)
{
    // Setup XT1 and XT2
	while(BAKCTL & LOCKBAK) {                 // Unlock XT1 pins for operation
		BAKCTL &= ~(LOCKBAK);                 // Battery Backup Control
	}

    XT2_PORT_SEL |= XT2_ENABLE;               // P7SEL |= (BIT2 + BIT3)
    UCSCTL6 &= ~(XT1OFF + XT2OFF);            // Set XT2 On
    UCSCTL6 |= XCAP_3;                        // Internal load cap

	UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);// Clear XT2,XT1,DCO fault flags
	SFRIFG1 &= ~OFIFG;                         // Clear fault flags

    // Port Configuration
    P1OUT = 0x00;P2OUT = 0x00;P3OUT = 0x00;P4OUT = 0x00;P5OUT = 0x00;P6OUT = 0x00;
    P7OUT = 0x00;P8OUT = 0x00;P9OUT = 0x00;PJOUT = 0x00;
    P1DIR = 0xFF;P2DIR = 0xFF;P3DIR = 0xFF;P4DIR = 0xFF;P5DIR = 0xFF;P6DIR = 0xFF;
    P7DIR = 0xFF;P8DIR = 0xFF;P9DIR = 0xFF;PJDIR = 0xFF;

    // Configure LED ports
    P9OUT &= ~(BIT6 + BIT7);          // P9OUT
    P9DIR |= BIT6 + BIT7;             // P9DIR

}

/***************************************************************************//**
 * @brief  Turn on LEDs
 * @param  ledMask   Use values defined in HAL_board.h for the LEDs to turn on
 * @return none
 ******************************************************************************/

void Board_ledOn(unsigned int ledMask)
{
    if (ledMask & LED1) LED12_PORT_OUT |= BIT6;
    if (ledMask & LED2) LED12_PORT_OUT |= BIT7;
}

/***************************************************************************//**
 * @brief  Turn off LEDs
 * @param  ledMask   Use values defined in HAL_board.h for the LEDs to turn off
 * @return none
 ******************************************************************************/

void Board_ledOff(unsigned int ledMask)
{
	if (ledMask & LED1) LED12_PORT_OUT &= ~BIT6;
	if (ledMask & LED2) LED12_PORT_OUT &= ~BIT7;
}

/***************************************************************************//**
 * @brief  Toggle LEDs
 * @param  ledMask   Use values defined in HAL_board.h for the LEDs to toggle
 * @return none
 ******************************************************************************/

void Board_ledToggle(unsigned int ledMask)
{
	if (ledMask & LED1) LED12_PORT_OUT ^= BIT6;
	if (ledMask & LED2) LED12_PORT_OUT ^= BIT7;
}
