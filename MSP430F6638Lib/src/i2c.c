/* 
* @FileName: i2c.c
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:15
* @Description: i2c������������Ҳ����ֲ��֮ǰpic��дeeprom�Ĵ��룬Э��ʵ�ֱȽ�����
*               ��Բ�ͬ������eeprom��TMP112�ȣ���΢�޸Ķ�д��ַ����ʱ����ʵ�ּ���
* @Modified by  |  Modified time  |  Description 
*  
*/

#include "../inc/i2c.h"

//P9.2��P9.3 as I2C's SDA and SCL for I2C
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


/*��ַ����д��������*/
#define OP_WRITE 0x90
#define OP_READ  0x91

unsigned char eflag = 0;
int ack_count_w = 0;
int ack_count_r = 0;


/*��ʱ�ӳ�������I2CЭ����ɶ�������*/
void usr_delay()
{
    int i;
	i = 100;   //25MHzʱΪ4ms
	while(--i);
}

/*��ʼ�źţ�ʱ���ź�Ϊ���ڼ䣬��SDA����Ϊ��ʼ�ź�*/
void i2c_start()
{
	//SDA_PULLUP;
	//SCL_PULLUP;
	I2C_SDA_O;      //����Ϊ���
	I2C_SCL_O;
	I2C_SCL_L;      //SCL����,TMP112�ͷ����ߣ��ȴ���ʼ�źţ�SCLƵ�ʲ��ܵ���1KHz������TMP112���Ḵλ����
    _nop();_nop();
    I2C_SDA_H;
	_nop(); _nop(); _nop();
	I2C_SCL_H;
	_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();
	I2C_SDA_L;
	_nop();_nop();_nop();_nop();_nop();_nop();_nop(); //SDA���ͺ󱣳�  > 100ns
	I2C_SCL_L;                                       //ʱ��Ϊ��ʱ����SDAд
	_nop(); _nop();_nop();_nop();_nop();_nop();       //Tlow  > 1300ns
}

/*ֹͣ�źţ�ʱ���ź�Ϊ���ڼ䣬��SDA����Ϊֹͣ�ź�*/
void i2c_stop()
{
    I2C_SDA_O;     // output
    I2C_SDA_L;
	_nop();_nop();_nop();_nop();
	I2C_SCL_H;
	_nop();_nop();_nop();_nop();_nop();_nop();_nop(); //stop ���� >100ns
	I2C_SDA_H;
	_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();
    I2C_SCL_L;
    _nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();  //stop ����һ��start����600ns
}

/*************************************
*�������ƣ�i2c_read
*�������ܣ���ȡ����
*��ڲ�����
*���ڲ�����read_data
*************************************/
unsigned char i2c_read()
{
    unsigned char i, read_data;
	I2C_SDA_I;                 //����Ϊ���룬׼����ȡ����
	for(i=0; i<8; i++)
	{
	    _nop();_nop();_nop();
		I2C_SCL_H;             //��SCL���߿�ʼ��ȡ�ź�
		_nop();_nop();_nop();_nop();_nop(); _nop(); _nop(); _nop();_nop();
		read_data <<= 1;       //����һλ
		if(BIT1 == I2C_SDA_V)
		{
		    read_data = read_data + 1;
		}
		_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();
		I2C_SCL_L;
	}
	I2C_SDA_O;                 //����Ϊ���������Ӧ��
	I2C_SDA_L;                 //Ӧ��
	_nop();_nop();_nop();
	I2C_SCL_H;                 //�ھŸ�ʱ�ӣ��öԷ���ȡӦ��
	_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();  //�ȴ��Է�����Ӧ��
    I2C_SCL_L;
    _nop(); _nop(); _nop(); _nop();_nop();_nop(); _nop(); _nop(); _nop();_nop();
	return (read_data);
}

/*************************************
*�������ƣ�i2c_HS
*�������ܣ��л���HSģʽ
*��ڲ�����write_data
*���ڲ�����ack_bit
*************************************/
void i2c_HS(void)
{
    unsigned char i;
    unsigned char write_data = 0x0f;
	I2C_SDA_O;                            //д��������Ϊ���
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
		_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();   //���ݽ���ʱ��  > 100ns
		I2C_SCL_H;
		_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();   //T high > 600ns
		I2C_SCL_L;
		_nop();                                             //���ݱ���ʱ��   > 0ns
        _nop();_nop(); _nop(); _nop(); _nop(); _nop();_nop();_nop();_nop();_nop();     //SCL ʱ�ӵ����� > 1300ns
		write_data <<= 1;
	}
	I2C_SCL_L;                                               //�л�������ģʽ��д��ɣ�SCL���ͣ���ACK
	_nop();_nop();_nop(); _nop(); _nop(); _nop();_nop();_nop();_nop();
}

/*************************************
*�������ƣ�i2c_write
*�������ܣ�д������
*��ڲ�����write_data
*���ڲ�����ack_bit
*************************************/
void i2c_write(unsigned char write_data)
{
    unsigned char i;
	I2C_SDA_O;                  //д��������Ϊ���
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
		_nop();_nop();_nop();_nop();_nop();        //���ݽ���ʱ��  > 100ns
		I2C_SCL_H;
		_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();_nop();    //T high > 600ns
		I2C_SCL_L;
		_nop();                                             //���ݱ���ʱ��   > 0ns
        _nop();_nop(); _nop(); _nop(); _nop(); _nop();_nop();_nop();_nop();_nop();     //SCL ʱ�ӵ����� > 1300ns
		write_data <<= 1;
	}
	I2C_SDA_I;             //����Ϊ����,׼����ȡӦ��
	_nop(); _nop(); _nop();_nop();_nop();_nop();
	I2C_SCL_H;
	_nop(); _nop(); _nop();_nop();_nop();_nop();
	if(BIT1 == I2C_SDA_V)       //��ȡӦ��
    {
        eflag = 1;         //û���յ�Ӧ��
    }
	else
	{
	    eflag = 0;        //�յ�Ӧ��
	}
	I2C_SCL_L;              //д��ɣ�SCL����
	_nop();_nop();_nop(); _nop(); _nop(); _nop(); _nop(); _nop();_nop();
}

/*************************************
*�������ƣ�i2c_write_to_addr
*�������ܣ�д�����ݵ�ָ���ĵ�ַ
*��ڲ�����addr, WDataH, WDataL
*���ڲ�����
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
*�������ƣ�i2c_read_by_addr
*�������ܣ���ָ����ַ��ȡ����
*��ڲ�����addr
*���ڲ�����void
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
