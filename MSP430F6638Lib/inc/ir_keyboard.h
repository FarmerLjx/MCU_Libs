/* 
* @FileName: ir_keyboard.h
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:15
* @Description: 红外遥控键盘驱动，兼容不同红外遥控主要修改 IR_CNT_OFFSET 的值
*               不同键盘的编码也许也会有不同，先回去不同按键的编码，然后定义对应的宏即可
* @Modified by  |  Modified time  |  Description 
*  
*/

#ifndef __IR_KEYBOARD_H__
#define __IR_KEYBOARD_H__

#include <msp430.h>


#define IR_CAP_CYCLE   (30)  //30 us = 1 / 32.768kHz
#define IR_CNT_OFFSET  (500) //15ms - DIF = IR_CNT_OFFSET * IR_CAP_CYCLE

#define IR_PREAMBLE   (13500)
#define IR_CONTINUE   (11250)
#define IR_H          (2245)
#define IR_L          (1125)

#define IrJudge(x, mode) ((x > ((mode - IR_CNT_OFFSET) / IR_CAP_CYCLE)) \
                         && (x < ((mode + IR_CNT_OFFSET) / IR_CAP_CYCLE)))

//  IR 协议接收 宏定义
#define IR_WAITING    (1)
#define IR_RECIEVING  (2)

// 键盘输入识别 
#define STATUS_SELECT_MODE     (1)  // 选择功能
#define MODE_GET_NUM           (2)  // 获取输入的数据


// 用于区分不同键盘
#define KEY_BOARD_NEW  // 新遥控键盘

// Number Key
#define KEY_0         (0x68)
#define KEY_1         (0x30)
#define KEY_2         (0x18)
#define KEY_3         (0x7a)
#define KEY_4         (0x10)
#define KEY_5         (0x38)
#define KEY_6         (0x5a)
#define KEY_7         (0x42)
#define KEY_8         (0x4a)
#define KEY_9         (0x52)

// 以下按键的编码不同遥控键盘有些不一样
#ifndef KEY_BOARD_NEW

// Media Key
#define KEY_VOL_UP    (0x90)
#define KEY_VOL_DOWN  (0xa8)
#define KEY_NEXT      (0xc2)  // 旧键盘的快进
#define KEY_PREV      (0x02)  // 旧键盘的快退
#define KEY_START_OR_STOP     (0x22)
#define KEY_MUTE      (0xe2)
#define KEY_POWER     (0xa2)

// Function Key
#define KEY_EQ        (0xe0)
#define KEY_MODE      (0x62)
#define KEY_RPT       (0x98)
#define KEY_USD       (0xb0)

#endif

// Media Key
#define KEY_VOLUP     (0xa8)
#define KEY_VOLDOWN   (0xe0)  // 和旧键盘的EQ冲突
#define KEY_FF        (0x02)  // Fast Forward
#define KEY_FR        (0x22)  // Fast Reverse 
#define KEY_PLAY_OR_PAUSE     (0xc2)
#define KEY_CH        (0x62)
#define KEY_CH_ADD    (0xa2)
#define KEY_CH_MINUS  (0xe2)

// Function Key
#define KEY_EQ        (0x90)
#define KEY_PLUS_100  (0x98)
#define KEY_PLUS_200  (0xb0)


void IrInit(void);
void JudgeAndProc(char key);
void JudgeAndDisplayKeys(char key);

#endif //
