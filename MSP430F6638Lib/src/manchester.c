/* 
* @FileName: manchester.c
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:15
* @Description: 曼切斯特编码实现，包含master和slaver的全部代码，需要移植要懂原理
*               通过MAN_MASTER预处理宏实现编译 master 或者 slaver
* @Modified by  |  Modified time  |  Description 
*  
*/

#include "../inc/manchester.h"
#include "../inc/lcd_12864.h"
#include "../inc/delay.h"

#define  MAN_MASTER     //发送和接收端的中断处理不同，使用预处理宏实现选择不同的ISR，当前选择：发送端
//#define  MAN_SLAVER     //接收端

// Manchester transport parameters
unsigned char g_ucParity = 0;

#ifdef MAN_MASTER
// TX
unsigned int  g_uiTXData = 0;
unsigned char g_ucTXCnt = 0;
unsigned char g_ucTXIndex = 0;
unsigned char g_ucTXState = SEND_NOTHING;

#else
// RX
unsigned char g_ucRXData = 0;
unsigned char g_ucRXCnt = 0;
unsigned char g_ucRXIndex = 0;
unsigned char g_ucRXState = WAITING_S;
unsigned char g_ucRXPreBit = 0;
unsigned char g_ucRXCurBit = 0;

#endif

/*
 * 初始化
** Used TB0, P3.4
*/
void InitManchester(void)
{
	//init a timer to count for Manchester, Set the timer B0 to 1MHz to timing 1us
    TBCCR0 = 1250;                      //SMCLK is 12.5MHz
    TBCCTL0 = CCIE;
	TBCTL = TBSSEL_2 + TBCLR;  // SMCLK, up mode, clear TBR

#ifdef MAN_MASTER
	// 设置输出端口
	P3SEL &= ~BIT4;   //I/O
	P3DIR |= BIT4;    //输出
	
	START_TB;
#else
	// 设置输入端口
	P3SEL &= ~BIT5;   //I/O
	P3DIR &= ~BIT5;   //输入
	P3IES &= ~BIT5;   //上升沿
	MAN_RX_EINT;
	
	STOP_TB;
#endif

}

#ifdef MAN_MASTER
// 用作发送端时的函数定义
// 初始化传输的数据
void InitTXData(unsigned char ucData)
{
	int i;
	int iParity = 0;
	unsigned int uiTXData = 0;

	DisplayByteInHex(ucData, 1, 1);

	for(i=0; i<8; ++i){
		//如果需要发送1，先置为零，再拉高，获得一个上升沿
		if((ucData << i) & 0x80){
			uiTXData <<= 1;
			uiTXData += 0;   // 置零
			uiTXData <<= 1;
			uiTXData += 1;   // 拉高
			iParity ^= 1;
		}
		//如果需要发送0，则先置一，再拉低，获得一个下降沿
		else{
			uiTXData <<= 1;
			uiTXData += 1;    //置一
			uiTXData <<= 1;
			uiTXData += 0;    //拉低
			iParity ^= 0;
		}
	}

	//test code
	DisplayByteInHex((unsigned char)(uiTXData &0x00ff), 3, 6);
	DisplayByteInHex((unsigned char)((uiTXData >> 8) &0x00ff), 3, 1);

	// 查询发送状态
	while(g_ucTXState != SEND_NOTHING);
	// 开始发送数据
	g_ucTXCnt = 0;
	g_uiTXData = uiTXData;
	g_ucParity = iParity;
	g_ucTXState = SENDING_DATA;
}

// ucIndex is used to confirm which byte to send
// | 0:S | 1-8:DATA | 9:PARITY | 10:P |
void ManSendData(unsigned char ucIndex)
{
	// 发送开始位 "0"
	if(1 == ucIndex){
		MAN_TX_H;
	}
	else if(2 == ucIndex){
		MAN_TX_L;
	}
	// 发送8位数据 3-18
	else if(ucIndex>=3 && ucIndex<=18){
		if((g_uiTXData << (ucIndex - 3)) & 0x8000){
			MAN_TX_H;
		}
		else{
			MAN_TX_L;
		}
	}
	// 发送校验位
	else if(19 == ucIndex){
		if(1 == g_ucParity){
			MAN_TX_L;
		}
		else{
			MAN_TX_H;
		}
	}
	else if(20 == ucIndex){
		if(1 == g_ucParity){
			MAN_TX_H;
		}
		else{
			MAN_TX_L;
		}
	}
	// 发送停止位 "1"
	else if(21 == ucIndex){
		MAN_TX_L;
	}
	else if(22 == ucIndex){
		MAN_TX_H;
	}
	// Reset
	else{
		MAN_TX_L;
		g_ucTXState = SEND_NOTHING;
		g_ucTXCnt = 0;
	}
}

#else
// 用作接收端时的函数定义
void ManGetData(unsigned char ucIndex)
{
	// 首先获取接收端口的电平，"0"或者"1"
	if(P3IN & BIT5){     // 如果收到的是1
		g_ucRXCurBit = 1;
	}
	else{
		g_ucRXCurBit = 0;
	}
	
	// 在Index为偶数的时候可以下定结论，确定收到的曼切斯特编码是"0"或者"1"
	if(!(ucIndex % 2)){
		// 确认是否为开始位
		if(2 == ucIndex){
			// 是开始位"0"
			if(1==g_ucRXPreBit && 0==g_ucRXCurBit){
				// Doing nothing
			}
			// 不是开始位
			else{
				g_ucRXState = WAITING_S; //等待开始位
				g_ucRXIndex = 0;
				STOP_TB;                 // 关闭定时器
				MAN_RX_EINT;
			}
		}
		// 接收8位数据
		else if(ucIndex>=4 && ucIndex <=18){
			g_ucRXData <<= 1;
			// "1"
			if(0==g_ucRXPreBit && 1==g_ucRXCurBit){
				g_ucRXData += 1;
				g_ucParity ^= 1;
			}
			// "0"
			else if(1==g_ucRXPreBit && 0==g_ucRXCurBit){
				g_ucRXData += 0;
				g_ucParity ^= 0;
			}
			// 出现误码
			else{
				g_ucRXState = WAITING_S; //等待开始位
				g_ucRXIndex = 0;
				STOP_TB;                 // 关闭定时器
				MAN_RX_EINT;
			}
		}
		// 接收奇偶校验位
		else if(20 == ucIndex){
			if(0==g_ucRXPreBit && 1==g_ucRXCurBit){
				g_ucParity ^= 1;
			}
			// "0"
			else if(1==g_ucRXPreBit && 0==g_ucRXCurBit){
				g_ucParity ^= 0;
			}
			// 不添加else判断，放宽标准
			// 检验校验码是否正确
			if(!g_ucParity){
				// 收到正确的数据
				DisplayByteInHex(g_ucRXData, 1, 1);
			}
			else{
				// 收到的数据不正确
				//DisplayByteInHex(g_ucRXData, 1, 1);
			}
		}
		// 接收停止位
		else if(22 == ucIndex){
			// "1"
			if(0==g_ucRXPreBit && 1==g_ucRXCurBit){
				// 收到结束位
			}
			// "0"
			else if(1==g_ucRXPreBit && 0==g_ucRXCurBit){
				//没有收到结束位
			}
		}
		// 通信结束
		else{
			g_ucRXState = WAITING_S; //等待开始位
			g_ucRXIndex = 0;
			STOP_TB;                 // 关闭定时器
			MAN_RX_EINT;             // 重新使能接收引脚中断
		}
	}
	// 获取连续的两位bit,用于确定一位曼切斯特编码
	else{
		g_ucRXPreBit = g_ucRXCurBit;
	}
	
}
#endif

//Timing 50us
#ifdef MAN_MASTER

#pragma vector=TIMERB0_VECTOR
__interrupt void TIMERB0_ISR (void)
{
	//P3OUT |= BIT4;

	switch(g_ucTXState){
	//没有数据传输
	case SEND_NOTHING:
		g_ucTXIndex = 0;
		MAN_TX_L;
		break;
	//发送数据
	case SENDING_DATA:
		++g_ucTXIndex;   //在这个case里面，调用ManSendData时g_ucTXIndex一定是从1开始
		ManSendData(g_ucTXIndex);
		break;
	//复位
	default:
		g_ucTXState = SEND_NOTHING;
		g_ucTXIndex = 0;
		break;
	}
}

#else

#pragma vector=TIMERB0_VECTOR
__interrupt void TIMERB0_ISR (void)
{
	++g_ucRXIndex;
	ManGetData(g_ucRXIndex);
}
#endif

#pragma vector=PORT3_VECTOR
__interrupt void PORT3_ISR (void)
{
	if(P3IFG & BIT5){
		//P9OUT ^= BIT6;
		P3IFG &= ~BIT5;
		//P3IES ^= BIT5;     // 检测双边沿
		
#ifdef MAN_MASTER
		_nop();
#else
		if(WAITING_S == g_ucRXState){
			MAN_RX_DINT;                   // 关闭接收端口的中断使能
			g_ucRXState = RECEIVING_DATA;  // 可能收到数据，下一步确认是否为开始位
			g_ucRXIndex = 1;               // 收到的数据位数清零，从1开始
			g_ucParity = 0;                // 奇偶校验清零
			for(i=0; i<SAMP_DELAY; ++i){
				__no_operation();
			}
			START_TB;                      // 打开定时器，定时一个周期去获取一次数据
			ManGetData(g_ucRXIndex);       // 采集第一位数据
		}
		
#endif
	}
}

