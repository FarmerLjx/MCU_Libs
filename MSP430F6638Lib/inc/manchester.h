/* 
* @FileName: manchester.h
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:15
* @Description: ����˹�ر��봫���ʵ�֣��ѷ��ͺͽ���д��һ��ģ�ʹ�õ���Ҫע��
*               Ҳ���Խ������ַֿ������������
* @Modified by  |  Modified time  |  Description 
*  
*/

#ifndef __MANCHESTER_H__
#define __MANCHESTER_H__

#include <msp430.h>

// Macro for Manchester
#define  STOP_TB         (TBCTL &= 0xffcf)    // MC�ǵ�4��5λ�������Թرն�ʱ��B
#define  START_TB        (TBCTL |= 0x0010)    // ��UP MODE������ʱ��B
#define  SAMP_DELAY      (125)                // �յ�������֮�����ʱ��ʱ��Ϊ *40ns��Ȼ��ʼÿ��һ��������ȡ����
// TX
#define  SEND_NOTHING    (0x0)
#define  SENDING_DATA    (0x1)
// RX
#define  WAITING_S       (0x2)
#define  RECEIVING_DATA  (0x3)

#define  MAN_TX_H        (P3OUT |= BIT4)    //���
#define  MAN_TX_L        (P3OUT &= ~BIT4)
#define  MAN_RX_H        (P3IN & BIT5)      //�����ж�
#define  MAN_RX_L        (!(P3IN & BIT5))
#define  MAN_RX_EINT     (P3IE |= BIT5)     //ʹ�������ж�
#define  MAN_RX_DINT     (P3IE &= ~BIT5)    //��ֹ�����ж�

// function list
void InitManchester(void);
void InitTXData(unsigned char ucData);
void ManSendData(unsigned char ucIndex);
void ManGetData(unsigned char ucIndex);


#endif  //!__MANCHESTER_H__
