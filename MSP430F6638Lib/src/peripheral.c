/* 
* @FileName: peripheral.c
* @Author  : PeeNut
* @Date    : 2015-08-08 20:42:45
* @Description: MSP430外设的初始化模块，ISR可以定义在该文件，也可以定义在其他文件中
*               通过ISR_INLINE和ISR_OUTLINE宏实现
* @Modified by  |  Modified time  |  Description 
*  
*/

#include "../inc/peripheral.h"

// If the ISR is defined this file
//#define ISR_INLINE
#define ISR_OUTLINE

// ADC 初始化程序
// 初始化使用A0-A3，ref为AVcc，使用SEQ转换序列，
void ADCInit(void)
{
	volatile unsigned int i;
	// Set Analog input
	P6SEL |= 0x0f;   // Enable A/D channel A0-A3 - P6.0-P6,3

	REFCTL0 &= ~REFMSTR; // Reset REFMSTR to hand over control to
	                     // ADC12_A ref control registers
	ADC12CTL0 = ADC12ON + ADC12MSC + ADC12SHT0_2;           // Turn on ADC12, set sampling time
	ADC12CTL1 = ADC12SHP + ADC12CONSEQ_1;                   // Use sampling timer
	ADC12MCTL0 = ADC12SREF_0 + ADC12INCH_0;                 // ref+=AVcc, channel = A0
	ADC12MCTL1 = ADC12SREF_0 + ADC12INCH_1;                 // ref+=AVcc, channel = A1
	ADC12MCTL2 = ADC12SREF_0 + ADC12INCH_2;                 // ref+=AVcc, channel = A2
	ADC12MCTL3 = ADC12SREF_0 + ADC12INCH_3+ADC12EOS;        // ref+=AVcc, channel = A3, end seq.
	ADC12IE = 0x08;                           // Enable ADC12IFG.3
	ADC12CTL0 |= ADC12ENC;                    // Enable conversions

}

// 比较器的初始化，设置比较引脚，和比较结果输出引脚；可能需要适当延时，等待参考电压建立
void CompInit(void)
{
	//设置比较器, CB0(P6.0)输入，P3.0输出，如果大于2V，输出为高
	P3DIR |= BIT0;                            // P3.0 output direction
	P3SEL |= BIT0;                            // Select CBOUT function on P3.0/CBOUT

	CBCTL0 |= CBIPEN + CBIPSEL_0;             // Enable V+, input channel CB0
	CBCTL1 |= CBPWRMD_1;                      // normal power mode
	CBCTL2 |= CBRSEL;                         // VREF is applied to -terminal
	CBCTL2 |= CBRS_3+CBREFL_2;                // R-ladder off; Bandgap voltage amplifier ON and generates 2.0V reference

	CBCTL3 |= CBPD0;                          // Input Buffer Disable @P6.0/CB0
	CBCTL1 |= CBON;                           // Turn On ComparatorB
	//delay_ms(1);                              // delay for the reference to settle
}


// UART初始化，需要用到端口布局
void UARTInit(void)
{
	Port_Mapping();                           //把I/O口设置为串口功能
    //把P2.0设置为TX，P2.1设置为RX
    P2SEL |= 0x03;                            // Assign P2.0 to UCA0TXD and...
	P2DIR |= 0x03;                            // P2.1 to UCA0RXD
	UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
	UCA0BR0 = 0x03;                           // 32kHz/9600=3.41 (see User's Guide)
	UCA0BR1 = 0x00;                           //
	UCA0MCTL = UCBRS_3+UCBRF_0;               // Modulation UCBRSx=3, UCBRFx=0
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

// 端口布局
void Port_Mapping(void)
{
  // Disable Interrupts before altering Port Mapping registers
  __disable_interrupt();
  // Enable Write-access to modify port mapping registers
  PMAPPWD = 0x02D52;

  #ifdef PORT_MAP_RECFG
  // Allow reconfiguration during runtime
  PMAPCTL = PMAPRECFG;
  #endif

  P2MAP0 = PM_UCA0TXD;
  P2MAP1 = PM_UCA0RXD;

  // Disable Write-Access to modify port mapping registers
  PMAPPWD = 0;
  #ifdef PORT_MAP_EINT
  __enable_interrupt();                     // Re-enable all interrupts
  #endif
}



#ifdef ISR_INLINE  // ISR is defined in main.c

// ADC12 ISR
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
	float temp1, temp2, temp;
    switch(__even_in_range(ADC12IV,34))
	{
		case  0: break;                           // Vector  0:  No interrupt
		case  2: break;                           // Vector  2:  ADC overflow
		case  4: break;                           // Vector  4:  ADC timing overflow
		case  6: break;                           // Vector  6:  ADC12IFG0
		case  8: break;                           // Vector  8:  ADC12IFG1
		case 10: break;                           // Vector 10:  ADC12IFG2
		case 12:                                  // Vector 12:  ADC12IFG3
			results[0] = ADC12MEM0;               // Move results, IFG is cleared
			results[1] = ADC12MEM1;
			results[2] = ADC12MEM2;
			results[3] = ADC12MEM3;
			__bic_SR_register_on_exit(LPM0_bits); // Exit active CPU, SET BREAKPOINT HERE 
			break;
		case 14: break;    // Vector 14:  ADC12IFG4
		case 16: break;    // Vector 16:  ADC12IFG5
		case 18: break;    // Vector 18:  ADC12IFG6
		case 20: break;    // Vector 20:  ADC12IFG7
		case 22: break;    // Vector 22:  ADC12IFG8
		case 24: break;    // Vector 24:  ADC12IFG9
		case 26: break;    // Vector 26:  ADC12IFG10
		case 28: break;    // Vector 28:  ADC12IFG11
		case 30: break;    // Vector 30:  ADC12IFG12
		case 32: break;    // Vector 32:  ADC12IFG13
		case 34: break;    // Vector 34:  ADC12IFG14
		default: break; 
	}
}

#endif
