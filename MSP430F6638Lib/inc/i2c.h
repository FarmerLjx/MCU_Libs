/* 
* @FileName: i2c.h
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:15
* @Description: i2c驱动模块，引脚定义在c文件中；需要移植时更改合适引脚和读写命令宏OP_
* @Modified by  |  Modified time  |  Description 
*  
*/


#ifndef  __I2C_H__
#define  __I2C_H__

#include <msp430.h>
#include <stdint.h>

void usr_delay();
void i2c_start();
void i2c_stop();
unsigned char i2c_read();
void i2c_HS(void);
void i2c_write(unsigned char write_data);
void i2c_write_to_addr(unsigned char addr, unsigned char WDataH, unsigned char WDataL);
int i2c_read_by_addr(unsigned char addr);


#endif  //!__I2C_H__
