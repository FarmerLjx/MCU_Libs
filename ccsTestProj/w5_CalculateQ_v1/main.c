/*
 * Author: ChaiHsiang
 * Date  : 20150808
 *
 */

#include <msp430.h> 
#include <stdint.h>
#include <math.h>
#include "./MSP430F6638Lib/inc/HAL_Board.h"
#include "./MSP430F6638Lib/inc/HAL_PMM.h"
#include "./MSP430F6638Lib/inc/HAL_UCS.h"
#include "./MSP430F6638Lib/inc/lcd_12864.h"
#include "./MSP430F6638Lib/inc/manchester.h"
#include "./MSP430F6638Lib/inc/delay.h"
#include "./MSP430F6638Lib/inc/ad9954.h"
#include "./MSP430F6638Lib/inc/ir_keyboard.h"
#include "./MSP430F6638Lib/inc/q_value.h"

#define PI  (3.1415926535)

unsigned char g_ucTest = 0;
unsigned char g_ucFlag = 1;

// 红外键盘输入结果
extern unsigned long g_ulGetNum;       // 用于通过无限遥控获得数字输入，范围为0-65535
extern unsigned char g_ucGetNumFlag;   // 用于确认用户输入完成，并且输入有效范围的数字

// ADC 转换结果
volatile unsigned int results[4];

// 界面显示需要的数据
unsigned long g_ulFrequence = 5000000;     // 范围是 20kHz - 45MHZ
double  g_dC = 0;                // 电容的值，单位是pF
double  g_dL = 0;                // 电感的值，单位是uL
double  g_dQ = 0;                // Q值

// 获取 电容-电压比例 表格数据
float g_fVRate = 0;

// Function list
void SysInit(void);
void ADSend(unsigned char dat);
void ADCInit(void);
void UIInit(void);
void UIRefresh(void);

void main(void)
{
   	SysInit();
    AD9954Init();
   	IrInit();
   	ADCInit();
   	UIInit();
	UIRefresh();
   	//InitManchester();

    //Set the timer A1
    TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
    TA1CCR0 = 3277;
    TA1CTL = TASSEL_1 + MC_1 + TACLR;         // SMCLK, up mode, clear TAR


    //AD9954_OSK_H;                           // OSK UP
    SingleTone();                             // 1MHz for default
	g_ulFrequence = 5000000;                  // 必须和SetFreqToHz绑定使用
    SetFreqToHz(g_ulFrequence);               // n Hz
	
	_EINT();

    while(1){
    	//ADSend(0xff);
    	_nop();_nop();_nop();_nop();_nop();
    	delay_ms(100);

    	if(g_ucGetNumFlag){
    		//g_ulFrequence = g_ulGetNum;
			//SetFreqToHz(g_ulFrequence);
			g_dC = GetCValueByVRate(g_ulGetNum / 1000.0);
			UIRefresh();
    		//AD9954SetReg8(ASF, 1000*g_ulGetNum);
    		//g_ulGetNum = 16415;
    		g_ucGetNumFlag = 0;      // Clear
    		g_ulGetNum = 0;          // Reset
			
			//g_dQ = GetCValueByVRate(g_fVRate);
    	}
		
		// 进入低功耗模式
		__bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, Enable interrupts
		__no_operation();                       // For debugger
    }
}

void SysInit(void)
{
    WDTCTL = WDTPW | WDTHOLD;	  // Stop watchdog timer
	Board_init();                 // Basic GPIO initialization
	SetVCore(3);                  // Set Vcore to accomodate for max. allowed system speed
	LFXT_Start(XT1DRIVE_0);       // Use 32.768kHz XTAL as reference, ACLK: SELA = 000, DIVA = 000
	Init_FLL_Settle(25000, 762);  // Set system clock to max (25MHz)

    LcdInit();
    LcdClear();
    //display_rect();
    AD9954Init();

    P4SEL = 0x00;
    P4DIR = 0x00;
    P4REN = 0xff;
}

// 初始化ADC为4通道，A0-A3同时测量转换，参考电压为AVcc
// 用 A0/A1 得到的电压比值的比值查 C-V1/V2 表获取电容
// 用 A2、A3 的电压值搜索另一张表
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

void UIInit(void)
{
	// 显示频率 符号
	disp_str(6, 0, "F=");  // F=00000kHZ
	// 显示电容 符号
	disp_str(4, 0, "C=");
	// 显示电感 符号
	disp_str(2, 0, "L=");
	// 显示Q值 符号
	disp_str(0, 0, "Q=");
}

void UIRefresh(void)
{
	int iRet = 0;
	
	// 更新频率显示
	disp_str(6, 2, "              ");  // 清空一行
	iRet = DisplayULongInDec(g_ulFrequence, 6, 2);
	disp_str(6, (iRet + 2), "Hz");
	
	// 更新电容显示
	disp_str(4, 2, "              ");
	iRet = DisplayDoubleInDec(g_dC, 3, 4, 2);
	disp_str(4, (2+iRet), "pF");
	
	// 更新电感显示
	if(g_dC != 0){
		g_dL = 1000000 / (g_dC * (4 * PI * PI * (g_ulFrequence / 1000) * (g_ulFrequence / 1000)) / 1000000);
	}
	else{
		g_dL = 0;
	}
	disp_str(2, 2, "              ");
	iRet = DisplayDoubleInDec(g_dL, 3, 2, 2);
	disp_str(2, (2+iRet), "uL");
	
	// 更新Q值显示
	disp_str(0, 2, "              ");
	iRet = DisplayDoubleInDec(g_dQ, 3, 0, 2);
	disp_str(0, (2+iRet), "  ");
}


// Freq Scan for a MAX Q-Value
// 频率精确到Hz，步长最大为2000000，即2MHz
int SearchForMaxQ(unsigned long ulStart, unsigned long ulStop, unsigned long ulStep)
{
	int i = 0;
	unsigned char ucFlag = 0;
	unsigned long ulFreq = 0;
	double dQValue = 0;
    double dQVTemp[4] = {0};   //
	
	if(ulStart > ulStop){
		return 0;
	}
	ulFreq = ulStart;
	
	// 从开始频率扫描到停止频率，每次增加 iStep
	for(i=0; ulFreq < ulStop; ++i){
		// 测量Q值
	    // ########在下面添加求Q值的逻辑###########and now it is a e...
		dQValue = 1000;
		
		// 小于3的时候
		if(0 == i){
			dQVTemp[i] = dQValue;
		}
		else if(i <= 3 && i >= 1){
			if(0 == ucFlag){
				
			}
			dQVTemp[i] = dQValue;
		}
		else{
			//if()
		}
	}
}


// Timer1 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
	ADC12CTL0 &= ~ADC12SC;
	ADC12CTL0 |= ADC12SC;                   // Start convn - software trigger
	__bic_SR_register_on_exit(LPM0_bits);
}

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
			//results[0] = 0x0fff;
			temp1 = (float)((results[0] * 3.3) / 0x0fff);
			results[1] = ADC12MEM1;
			//results[1] = 0x07c4;
			temp2 = (float)((results[1] * 3.3) / 0x0fff);
			//DisplayDoubleInDec(temp1, 4, 3, 1);
			//DisplayIntInHex(results[0], 3, 8);
			//DisplayDoubleInDec(temp2, 4, 1, 1);
			//DisplayIntInHex(results[1], 1, 8);
			if(temp2 > 0){
				temp = (float)(temp1 / temp2);
				if(g_fVRate < (temp + 0.04) && g_fVRate > (temp - 0.04)){
					;
				}
				else{
					g_fVRate = temp;
					// 根据获取到的电压比值得到
					g_dC = GetCValueByVRate(g_fVRate);
					// 更新电容显示
					UIRefresh();
				}
			}
			else{
				temp = 0;
			}
			
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

