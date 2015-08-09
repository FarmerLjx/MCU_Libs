/* 
* @FileName: manchester.h
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:15
* @Description: 曼切斯特编码传输的实现，把发送和接收写在一起的，使用的需要注意
*               也可以将两部分分开，条理更清晰
* @Modified by  |  Modified time  |  Description 
*  
*/

#ifndef __MANCHESTER_H__
#define __MANCHESTER_H__

#include <msp430.h>

// Macro for Manchester
#define  STOP_TB         (TBCTL &= 0xffcf)    // MC是第4、5位，清零以关闭定时器B
#define  START_TB        (TBCTL |= 0x0010)    // 以UP MODE启动定时器B
#define  SAMP_DELAY      (125)                // 收到上升沿之后的延时，时间为 *40ns，然后开始每隔一个周期收取数据
// TX
#define  SEND_NOTHING    (0x0)
#define  SENDING_DATA    (0x1)
// RX
#define  WAITING_S       (0x2)
#define  RECEIVING_DATA  (0x3)

#define  MAN_TX_H        (P3OUT |= BIT4)    //输出
#define  MAN_TX_L        (P3OUT &= ~BIT4)
#define  MAN_RX_H        (P3IN & BIT5)      //条件判断
#define  MAN_RX_L        (!(P3IN & BIT5))
#define  MAN_RX_EINT     (P3IE |= BIT5)     //使能引脚中断
#define  MAN_RX_DINT     (P3IE &= ~BIT5)    //禁止引脚中断

// function list
void InitManchester(void);
void InitTXData(unsigned char ucData);
void ManSendData(unsigned char ucIndex);
void ManGetData(unsigned char ucIndex);


#endif  //!__MANCHESTER_H__
