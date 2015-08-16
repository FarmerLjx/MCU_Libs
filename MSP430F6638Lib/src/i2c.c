/* 
* @FileName: i2c.c
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:15
* @Description: i2c的驱动，本来也是移植自之前pic读写eeprom的代码，协议实现比较完善
*               针对不同器件（eeprom、TMP112等）稍微修改读写地址和延时即能实现兼容
* @Modified by  |  Modified time  |  Description 
*   PeeNut         20150814 01:36     修改i2c_write函数的返回值，设置为表明是否收到ACK
*/

#include "../inc/i2c.h"
#include "../inc/delay.h"

// Port define for I2C
#define  I2C_SDA_V  (P3IN & BIT1)       //SDA as input's value
#define  I2C_SDA_I  (P3DIR &= ~BIT1)    //P3.1 as SDA in input
#define  I2C_SDA_O  (P3DIR |= BIT1)     //P3.1 as SDA in output
#define  I2C_SDA_H  (P3OUT |= BIT1)     //SDA as output in Hight
#define  I2C_SDA_L  (P3OUT &= ~BIT1)    //SDA as output in Low
#define  I2C_SCL_O  (P3DIR |= BIT0)     //P3.0 as SCL in output
#define  I2C_SCL_H  (P3OUT |= BIT0)     //P3.0 as SCL in Hight
#define  I2C_SCL_L  (P3OUT &= ~BIT0)    //P3.0 as SCL in Low

//enable pull up resistance
#define  SDA_PULLUP (P3REN |= BIT1)
#define  SCL_PULLUP (P3REN |= BIT0)


/*地址、读写操作定义*/
#define OP_WRITE 0x90
#define OP_READ  0x91

unsigned char eflag = 0;
int ack_count_w = 0;
int ack_count_r = 0;


/*延时子程序：用在I2C协议完成读操作后*/
void usr_delay()
{
    int i;
	i = 100;   //25MHz时为4us
	while(--i);
}

/*延时子程序：ms*/
void usr_delay_fi()
{
    delay_us(4);
}

/*开始信号：时钟信号为高期间，将SDA拉低为开始信号*/
void i2c_start()
{
	//SDA_PULLUP;
	//SCL_PULLUP;
	I2C_SDA_O;      //设置为输出
	I2C_SCL_O;
	I2C_SCL_L;      //SCL拉低,TMP112释放总线，等待开始信号，SCL频率不能低于1KHz，否则TMP112将会复位总线
    _nop();_nop();
    I2C_SDA_H;
	_nop(); _nop(); _nop();
	I2C_SCL_H;
	_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();
	I2C_SDA_L;
	_nop();_nop();_nop();_nop();_nop();_nop();_nop(); //SDA拉低后保持  > 100ns
	I2C_SCL_L;                                       //时钟为低时允许SDA写
	_nop(); _nop();_nop();_nop();_nop();_nop();       //Tlow  > 1300ns
}

/*停止信号：时钟信号为高期间，将SDA拉高为停止信号*/
void i2c_stop()
{
    I2C_SDA_O;     // output
    I2C_SDA_L;
	_nop();_nop();_nop();_nop();
	I2C_SCL_H;
	_nop();_nop();_nop();_nop();_nop();_nop();_nop(); //stop 建立 >100ns
	I2C_SDA_H;
	_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();
    I2C_SCL_L;
    _nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();  //stop 与下一个start至少600ns
}

/*************************************
*函数名称：i2c_read
*函数功能：读取数据
*入口参数：
*出口参数：read_data
*************************************/
unsigned char i2c_read()
{
    unsigned char i, read_data;
	I2C_SDA_I;                 //设置为输入，准备读取数据
	for(i=0; i<8; i++)
	{
	    _nop();_nop();_nop();
		I2C_SCL_H;             //将SCL拉高开始读取信号
		_nop();_nop();_nop();_nop();_nop(); _nop(); _nop(); _nop();_nop();
		read_data <<= 1;       //左移一位
		if(BIT1 == I2C_SDA_V)
		{
		    read_data = read_data + 1;
		}
		_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();
		I2C_SCL_L;
	}
	I2C_SDA_O;                 //设置为输出，发送应答
	I2C_SDA_L;                 //应答
	_nop();_nop();_nop();
	I2C_SCL_H;                 //第九个时钟，让对方读取应答
	_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();  //等待对方接受应答
    I2C_SCL_L;
    _nop(); _nop(); _nop(); _nop();_nop();_nop(); _nop(); _nop(); _nop();_nop();
	return (read_data);
}

/*************************************
*函数名称：i2c_HS
*函数功能：切换到HS模式
*入口参数：write_data
*出口参数：ack_bit
*************************************/
void i2c_HS(void)
{
    unsigned char i;
    unsigned char write_data = 0x0f;
	I2C_SDA_O;                            //写操作设置为输出
	for(i=0; i<8; i++)
	{
	    if(write_data & 0x80)
		{
		    I2C_SDA_H;
		}
		else
		{
		    I2C_SDA_L;
		}
		_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();   //数据建立时间  > 100ns
		I2C_SCL_H;
		_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();   //T high > 600ns
		I2C_SCL_L;
		_nop();                                             //数据保存时间   > 0ns
        _nop();_nop(); _nop(); _nop(); _nop(); _nop();_nop();_nop();_nop();_nop();     //SCL 时钟低周期 > 1300ns
		write_data <<= 1;
	}
	I2C_SCL_L;                                               //切换到高速模式，写完成，SCL拉低，无ACK
	_nop();_nop();_nop(); _nop(); _nop(); _nop();_nop();_nop();_nop();
}

/*************************************
*函数名称：i2c_write
*函数功能：写入数据
*入口参数：write_data
*出口参数：int 返回值表明是否收到应答，0-收到，1-没有收到
*************************************/
int i2c_write(unsigned char write_data)
{
    unsigned char i;
    int iRet = 0;

	I2C_SDA_O;                  //写操作设置为输出
	for(i=0; i<8; i++)
	{
	    if(write_data & 0x80)
		{
		    I2C_SDA_H;
		}
		else
		{
		    I2C_SDA_L;
		}
		_nop();_nop();_nop();_nop();_nop();        //数据建立时间  > 100ns
		I2C_SCL_H;
		_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();    //T high > 600ns
		I2C_SCL_L;
		_nop();                                             //数据保存时间   > 0ns
        _nop();_nop(); _nop(); _nop(); _nop(); _nop();_nop();_nop();_nop();_nop();     //SCL 时钟低周期 > 1300ns
		write_data <<= 1;
	}
	I2C_SDA_I;             //设置为输入,准备读取应答
	_nop(); _nop(); _nop();_nop();_nop();_nop();
	I2C_SCL_H;
	_nop(); _nop(); _nop();_nop();_nop();_nop();
	if(BIT1 == I2C_SDA_V)       //读取应答
    {
        iRet = 1;         //没有收到应答
    }
	else
	{
	    iRet = 0;        //收到应答
	}
	I2C_SCL_L;              //写完成，SCL拉低
	_nop();_nop();_nop(); _nop(); _nop(); _nop(); _nop(); _nop();_nop();

	return iRet;
}

/*************************************
*函数名称：i2c_write_to_addr
*函数功能：写入数据到指定的地址
*入口参数：addr, WDataH, WDataL
*出口参数：
*************************************/
void i2c_write_to_addr(unsigned char addr, unsigned char WDataH, unsigned char WDataL)
{
    i2c_start();
	i2c_write(OP_WRITE);
    //if(eflag == 1) ack_count_w++;
	i2c_write(addr);
    //if(eflag == 1) ack_count_w++;
	i2c_write(WDataH);
    _nop();
    i2c_write(WDataL);
    //if(eflag == 1) ack_count_w++;
	i2c_stop();
	usr_delay();
}

/*************************************
*函数名称：i2c_read_by_addr
*函数功能：向指定地址读取数据
*入口参数：addr
*出口参数：void
*************************************/
int i2c_read_by_addr(unsigned char addr)
{
	int value;
	unsigned char ucTmpH;
	unsigned char ucTmpL;
    //unsigned char read_data;
	i2c_start();
	i2c_write(OP_WRITE);
    if(eflag == 1) ack_count_r++;
	i2c_write(addr);
    if(eflag == 1) ack_count_r++;
	i2c_start();
	i2c_write(OP_READ);
    if(eflag == 1) ack_count_r++;
	//read_data = i2c_read();
    ucTmpH = i2c_read();
    _nop();
    ucTmpL = i2c_read();
	i2c_stop();

	value = (int)ucTmpH;
	value <<= 8;
	value += (int)ucTmpL;

	return value;
}
