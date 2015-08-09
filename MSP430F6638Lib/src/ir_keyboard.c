/* 
* @FileName: ir_keyboard.c
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:15
* @Description: ���ߺ���ң�ؼ���������ֻҪ�Ǻ���ң���źţ���ģ���Լ��޸ļ��ܼ���
*               ��Ҫ��Ҫ���ǵ�����Բ�ͬң������ �ź��ݲDIF���Ͳ�ͬ�����ı�����ܲ�һ��
* @Modified by  |  Modified time  |  Description 
*  
*/

#include "../inc/ir_keyboard.h"
#include "../inc/lcd_12864.h"

// Static parameters for IR
//static unsigned long g_ulIrValue;
static unsigned long g_ulIrValueCache;
static int g_iIRStatus = IR_WAITING;
static unsigned char g_ucIRCnt;
//static int g_iIRAddr;
static unsigned char g_ucClickCnt = 0;
static unsigned char g_ucCurKey = 0;
static int g_iIRValue;
//static unsigned char g_ucCnt = 0;          // �����������ֵ�λ��

// Extern parameters for other modules to use this IR moudle
unsigned long  g_ulGetNum = 0;       // ����ͨ������ң�ػ���������룬��ΧΪ0-65535
unsigned char g_ucGetNumFlag = 0;    // ����ȷ���û�������ɣ�����������Ч��Χ������

void IrInit(void)
{
	//TA0ʱ��ԴΪACLK-32kHz������������ģʽ�����㣬�ж�ʹ��
	TA0CTL = TASSEL__ACLK + MC__CONTINOUS + TACLR;
	//�����½��أ�����CCI1_B������ģʽ���ж�ʹ��
	TA0CCTL1 = CM_2 +CCIS_1+CAP+CCIE;

	P1SEL |= BIT6;
	P1DIR &= ~BIT6;
}

// Get user input and do some thing
void JudgeAndProc(char key)
{
	if(g_ucCurKey == key){
		++g_ucClickCnt;
	}
	else{
		g_ucCurKey = key;
		g_ucClickCnt = 1;
	}
	disp_word(2, 15, (g_ucClickCnt + 48));
	
	switch(key){
	case KEY_0:
		g_ulGetNum *= 10;
		g_ulGetNum += 0;
		disp_word(0, 15, 48);
		break;
	case KEY_1:
		g_ulGetNum *= 10;
		g_ulGetNum += 1;
		disp_word(0, 15, 49);
		break;
	case KEY_2:
		g_ulGetNum *= 10;
		g_ulGetNum += 2;
		disp_word(0, 15, 50);
		break;
	case KEY_3:
		g_ulGetNum *= 10;
		g_ulGetNum += 3;
		disp_word(0, 15, 51);
		break;
	case KEY_4:
		g_ulGetNum *= 10;
		g_ulGetNum += 4;
		disp_word(0, 15, 52);
		break;
	case KEY_5:
		g_ulGetNum *= 10;
		g_ulGetNum += 5;
		disp_word(0, 15, 53);
		break;
	case KEY_6:
		g_ulGetNum *= 10;
		g_ulGetNum += 6;
		disp_word(0, 15, 54);
		break;
	case KEY_7:
		g_ulGetNum *= 10;
		g_ulGetNum += 7;
		disp_word(0, 15, 55);
		break;
	case KEY_8:
		g_ulGetNum *= 10;
		g_ulGetNum += 8;
		disp_word(0, 15, 56);
		break;
	case KEY_9:
		g_ulGetNum *= 10;
		g_ulGetNum += 9;
		disp_word(0, 15, 57);
		break;
		
		// *1
	case KEY_EQ:
		g_ucGetNumFlag = 1;
		disp_word(0, 15, '#');
		break;
		
		// *1000
	case KEY_PLUS_100:
	    g_ulGetNum *= 1000;
	    g_ucGetNumFlag = 1;
		disp_word(0, 15, 'K');
		break;
		
		// *1000000
	case KEY_PLUS_200:
		g_ulGetNum *= 1000000;
		g_ucGetNumFlag = 1;
		disp_word(0, 15, 'M');
		break;
	default:
		g_ucGetNumFlag = 0;
		g_ulGetNum = 0;
		disp_word(0, 15, '-');
		break;
	}
}

void JudgeAndDisplayKeys(char key)
{
	disp_str(0, 10, "      ");
	disp_str(0, 10, "Pre.: ");
	switch(key){
	case KEY_0:
		disp_word(0, 15, 48);
		break;
	case KEY_1:
		disp_word(0, 15, 49);
		break;
	case KEY_2:
		disp_word(0, 15, 50);
		break;
	case KEY_3:
		disp_word(0, 15, 51);
		break;
	case KEY_4:
		disp_word(0, 15, 52);
		break;
	case KEY_5:
		disp_word(0, 15, 53);
		break;
	case KEY_6:
		disp_word(0, 15, 54);
		break;
	case KEY_7:
		disp_word(0, 15, 55);
		break;
	case KEY_8:
		disp_word(0, 15, 56);
		break;
	case KEY_9:
		disp_word(0, 15, 57);
		break;
	default:
		disp_word(0, 15, '-');
		break;
	}
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
	  static unsigned int  cap_pre,cap_cur;
	  unsigned int cap_dif;

	  switch (__even_in_range(TA0IV, TA0IV_TAIFG)){
	  case TA0IV_TACCR1:
		  cap_pre = cap_cur;
		  cap_cur = TA0CCR1;
		  if(cap_cur > cap_pre){
			  cap_dif = cap_cur - cap_pre;
		  }
		  else{
			  cap_dif = 65536 - (cap_pre - cap_cur);
		  }

		  if(IR_WAITING == g_iIRStatus){
			  if(IrJudge(cap_dif, IR_PREAMBLE)){
				  //ǰ���룬׼����������
				  g_iIRStatus = IR_RECIEVING;
				  g_ucIRCnt = 0;
			  }
			  else if(IrJudge(cap_dif, IR_CONTINUE)){
				  //�����룬�յ���ǰ��һ�µ�����
				  //g_ulIrValue = g_ulIrValueCache;
				  g_ucIRCnt = 0;
			  }
		  }
		  else if(IR_RECIEVING == g_iIRStatus){
			  //�ж���0����1
			  ++g_ucIRCnt;
			  if(IrJudge(cap_dif, IR_H)){
				  //�յ� 1
				  g_ulIrValueCache = (g_ulIrValueCache << 1) + 1;
			  }
			  else if(IrJudge(cap_dif, IR_L)){
				  //�յ� 0
				  g_ulIrValueCache <<= 1;
			  }
			  else{
				  g_iIRStatus = IR_WAITING;
				  g_ucIRCnt = 0;
			  }
			  //Recieved 32 bits
			  if(32 == g_ucIRCnt){
				  g_iIRValue = (int)(g_ulIrValueCache & 0x0000ffff);
				  //g_iIRAddr = (int)((g_ulIrValueCache >> 16) & 0x0000ffff);
				  g_iIRStatus = IR_WAITING;
				  //JudgeAndDisplayKeys((char)(g_iIRValue >> 8));
				  JudgeAndProc((char)(g_iIRValue >> 8));
				  //DisplayIntInHex(g_iIRValue, 5, 1);
				  //DisplayIntInHex(g_iIRAddr, 5, 8);
			  }
		  }
	  }
	  __bic_SR_register_on_exit(LPM0_bits);
}
