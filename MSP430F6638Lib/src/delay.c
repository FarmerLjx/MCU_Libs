/* 
* @FileName: delay.c
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:14
* @Description: �����ʱ������lcd12864����Ҫ����ʱ����Ҳ�������ṩ��׼ȷ��ʱ�ĺ������ȿ��ܲ��Ǻܸ�
* @Modified by  |  Modified time  |  Description 
*  
*/

#include <msp430.h>
#include <stdint.h>
#include "../inc/delay.h"

//CPU_FΪ��ǰϵͳʱ�ӵ���Ƶ
#define CPU_F ((double)25000000)
#define ONE_MS			6250
#define ONE_FOUR_O_US	875

void delay_ms(unsigned int n)
{
	unsigned long i;

	i = ((CPU_F * n) / 1000.0);

	for( ; i > 0; i--){
	    __no_operation();
	}
}

void delay_us(unsigned int n)
{
	unsigned long i;

	i = ((CPU_F * n) / 1000000.0);

	for( ; i > 0; i--){
	    __no_operation();
	}
}

void delay_1ms(unsigned int nValue)//delay (nValue * 1)ms
{
	unsigned int i;
	unsigned int j;
	for(i = nValue; i > 0;i--)
	{
		for(j = ONE_MS; j > 0; j--)
			__no_operation();
	}
}

//*********************************************************//
void delay_1us(unsigned int nValue)//delay (0.88 + nValue * 0.16)us
{
	unsigned int i = nValue - 1;
//	unsigned int j = ONE_US;

	__no_operation();

	while(i)
	{
//		while(j-- > 0){
			__no_operation();
//		}
		i--;
	}
}

//*********************************************************//
void delay_140us(unsigned int nValue)//delay (nValue*140)us
{
	unsigned int i;
	unsigned int j;
	for(i = nValue; i > 0; i--)
	{
		for(j = ONE_FOUR_O_US; j > 0;j--)
			__no_operation();
	}
}

