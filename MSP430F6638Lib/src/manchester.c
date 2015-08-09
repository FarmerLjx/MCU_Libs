/* 
* @FileName: manchester.c
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:15
* @Description: ����˹�ر���ʵ�֣�����master��slaver��ȫ�����룬��Ҫ��ֲҪ��ԭ��
*               ͨ��MAN_MASTERԤ�����ʵ�ֱ��� master ���� slaver
* @Modified by  |  Modified time  |  Description 
*  
*/

#include "../inc/manchester.h"
#include "../inc/lcd_12864.h"
#include "../inc/delay.h"

#define  MAN_MASTER     //���ͺͽ��ն˵��жϴ���ͬ��ʹ��Ԥ�����ʵ��ѡ��ͬ��ISR����ǰѡ�񣺷��Ͷ�
//#define  MAN_SLAVER     //���ն�

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
 * ��ʼ��
** Used TB0, P3.4
*/
void InitManchester(void)
{
	//init a timer to count for Manchester, Set the timer B0 to 1MHz to timing 1us
    TBCCR0 = 1250;                      //SMCLK is 12.5MHz
    TBCCTL0 = CCIE;
	TBCTL = TBSSEL_2 + TBCLR;  // SMCLK, up mode, clear TBR

#ifdef MAN_MASTER
	// ��������˿�
	P3SEL &= ~BIT4;   //I/O
	P3DIR |= BIT4;    //���
	
	START_TB;
#else
	// ��������˿�
	P3SEL &= ~BIT5;   //I/O
	P3DIR &= ~BIT5;   //����
	P3IES &= ~BIT5;   //������
	MAN_RX_EINT;
	
	STOP_TB;
#endif

}

#ifdef MAN_MASTER
// �������Ͷ�ʱ�ĺ�������
// ��ʼ�����������
void InitTXData(unsigned char ucData)
{
	int i;
	int iParity = 0;
	unsigned int uiTXData = 0;

	DisplayByteInHex(ucData, 1, 1);

	for(i=0; i<8; ++i){
		//�����Ҫ����1������Ϊ�㣬�����ߣ����һ��������
		if((ucData << i) & 0x80){
			uiTXData <<= 1;
			uiTXData += 0;   // ����
			uiTXData <<= 1;
			uiTXData += 1;   // ����
			iParity ^= 1;
		}
		//�����Ҫ����0��������һ�������ͣ����һ���½���
		else{
			uiTXData <<= 1;
			uiTXData += 1;    //��һ
			uiTXData <<= 1;
			uiTXData += 0;    //����
			iParity ^= 0;
		}
	}

	//test code
	DisplayByteInHex((unsigned char)(uiTXData &0x00ff), 3, 6);
	DisplayByteInHex((unsigned char)((uiTXData >> 8) &0x00ff), 3, 1);

	// ��ѯ����״̬
	while(g_ucTXState != SEND_NOTHING);
	// ��ʼ��������
	g_ucTXCnt = 0;
	g_uiTXData = uiTXData;
	g_ucParity = iParity;
	g_ucTXState = SENDING_DATA;
}

// ucIndex is used to confirm which byte to send
// | 0:S | 1-8:DATA | 9:PARITY | 10:P |
void ManSendData(unsigned char ucIndex)
{
	// ���Ϳ�ʼλ "0"
	if(1 == ucIndex){
		MAN_TX_H;
	}
	else if(2 == ucIndex){
		MAN_TX_L;
	}
	// ����8λ���� 3-18
	else if(ucIndex>=3 && ucIndex<=18){
		if((g_uiTXData << (ucIndex - 3)) & 0x8000){
			MAN_TX_H;
		}
		else{
			MAN_TX_L;
		}
	}
	// ����У��λ
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
	// ����ֹͣλ "1"
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
// �������ն�ʱ�ĺ�������
void ManGetData(unsigned char ucIndex)
{
	// ���Ȼ�ȡ���ն˿ڵĵ�ƽ��"0"����"1"
	if(P3IN & BIT5){     // ����յ�����1
		g_ucRXCurBit = 1;
	}
	else{
		g_ucRXCurBit = 0;
	}
	
	// ��IndexΪż����ʱ������¶����ۣ�ȷ���յ�������˹�ر�����"0"����"1"
	if(!(ucIndex % 2)){
		// ȷ���Ƿ�Ϊ��ʼλ
		if(2 == ucIndex){
			// �ǿ�ʼλ"0"
			if(1==g_ucRXPreBit && 0==g_ucRXCurBit){
				// Doing nothing
			}
			// ���ǿ�ʼλ
			else{
				g_ucRXState = WAITING_S; //�ȴ���ʼλ
				g_ucRXIndex = 0;
				STOP_TB;                 // �رն�ʱ��
				MAN_RX_EINT;
			}
		}
		// ����8λ����
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
			// ��������
			else{
				g_ucRXState = WAITING_S; //�ȴ���ʼλ
				g_ucRXIndex = 0;
				STOP_TB;                 // �رն�ʱ��
				MAN_RX_EINT;
			}
		}
		// ������żУ��λ
		else if(20 == ucIndex){
			if(0==g_ucRXPreBit && 1==g_ucRXCurBit){
				g_ucParity ^= 1;
			}
			// "0"
			else if(1==g_ucRXPreBit && 0==g_ucRXCurBit){
				g_ucParity ^= 0;
			}
			// �����else�жϣ��ſ��׼
			// ����У�����Ƿ���ȷ
			if(!g_ucParity){
				// �յ���ȷ������
				DisplayByteInHex(g_ucRXData, 1, 1);
			}
			else{
				// �յ������ݲ���ȷ
				//DisplayByteInHex(g_ucRXData, 1, 1);
			}
		}
		// ����ֹͣλ
		else if(22 == ucIndex){
			// "1"
			if(0==g_ucRXPreBit && 1==g_ucRXCurBit){
				// �յ�����λ
			}
			// "0"
			else if(1==g_ucRXPreBit && 0==g_ucRXCurBit){
				//û���յ�����λ
			}
		}
		// ͨ�Ž���
		else{
			g_ucRXState = WAITING_S; //�ȴ���ʼλ
			g_ucRXIndex = 0;
			STOP_TB;                 // �رն�ʱ��
			MAN_RX_EINT;             // ����ʹ�ܽ��������ж�
		}
	}
	// ��ȡ��������λbit,����ȷ��һλ����˹�ر���
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
	//û�����ݴ���
	case SEND_NOTHING:
		g_ucTXIndex = 0;
		MAN_TX_L;
		break;
	//��������
	case SENDING_DATA:
		++g_ucTXIndex;   //�����case���棬����ManSendDataʱg_ucTXIndexһ���Ǵ�1��ʼ
		ManSendData(g_ucTXIndex);
		break;
	//��λ
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
		//P3IES ^= BIT5;     // ���˫����
		
#ifdef MAN_MASTER
		_nop();
#else
		if(WAITING_S == g_ucRXState){
			MAN_RX_DINT;                   // �رս��ն˿ڵ��ж�ʹ��
			g_ucRXState = RECEIVING_DATA;  // �����յ����ݣ���һ��ȷ���Ƿ�Ϊ��ʼλ
			g_ucRXIndex = 1;               // �յ�������λ�����㣬��1��ʼ
			g_ucParity = 0;                // ��żУ������
			for(i=0; i<SAMP_DELAY; ++i){
				__no_operation();
			}
			START_TB;                      // �򿪶�ʱ������ʱһ������ȥ��ȡһ������
			ManGetData(g_ucRXIndex);       // �ɼ���һλ����
		}
		
#endif
	}
}

