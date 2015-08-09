/* 
* @FileName: q_value.h
* @Author  : PeeNut
* @Date    : 2015-08-08 19:48:15
* @Description: 主要实现了一个查找表函数，需要时可以做下修改就能使用，基于二分法
* @Modified by  |  Modified time  |  Description 
*  
*/

#ifndef __Q_VALUE_H__
#define __Q_VALUE_H__

#define C_TABLE_NUM    124   // 电容查找表的大小

float GetCValueByVRate(float fVRate);
int BinarySearchRecursion(float arry[],float value,int start,int end);

#endif  //! __Q_VALUE_H__
