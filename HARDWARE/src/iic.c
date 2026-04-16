#include "iic.h"

/*************************
函数名称：IIc_Init(void)
函数功能：IIc初始化配置函数 (IO 口模拟配置)
返回值：无
形参：无
作者：me
版本：1.0 
	IIC_SCL --- PB6 --- 通用推挽输出
	IIC_SDA --- PB7 --- 通用开漏输出				AHB1			
*************************/
void IIc_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
  GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//空闲状态
	IIC_SCL_H;
	IIC_SDA_H;
	
}

/*************************
函数名称：IIc_Start(void)
函数功能：IIc起始信号函数 
返回值：无
形参：无
作者：me
版本：1.0 
	在时钟线为高电平的时候去拉低数据线
*************************/
void IIc_Start(void)
{
	IIC_SDA_H;
	Delay_Us(us);
	
	IIC_SCL_H;
	Delay_Us(us);
	
	IIC_SDA_L;
	Delay_Us(us);
	
}

/*************************
函数名称：IIc_End(void)
函数功能：IIc结束信号函数 
返回值：无
形参：无
作者：me
版本：1.0 
	在时钟线为高电平的时候去拉高数据线
*************************/
void IIc_End(void)
{
	IIC_SDA_L;
	Delay_Us(us);
	
	IIC_SCL_H;
	Delay_Us(us);
	
	IIC_SDA_H;
	Delay_Us(us);
}

/*************************
函数名称：IIc_Send_Ack(u8 ack)
函数功能：IIc发送应答/不应答信号函数 
返回值：无
形参：u8 ack		0 发送应答 	1 发送不应答
作者：me
版本：1.0 
*************************/
void IIc_Send_Ack(u8 ack)
{
	IIC_SCL_L;
	Delay_Us(us);
	if(ack == 1)
	{
		IIC_SDA_H;
	}
	else
	{
		IIC_SDA_L;
	}
	Delay_Us(us);
	
	IIC_SCL_H;
	Delay_Us(us);
	
	IIC_SCL_L;
	Delay_Us(us);
}

/*************************
函数名称：IIc_Rec_Ack(void)
函数功能：IIc接收应答/不应答信号函数 
返回值：u8 ack		0 表示应答 	1 表示不应答
形参：无
作者：me
版本：1.0 				SCL推挽		SDA开漏
*************************/
u8 IIc_Rec_Ack(void)
{
	u8 ack;
	
	IIC_SDA_H;
	
	IIC_SCL_L;//拉低时钟线，从机向主机发送1bit应答
	Delay_Us(us);
	IIC_SCL_H;//拉高时钟线，主机接收刚刚从机发送的1bit应答
	Delay_Us(us);
	if(IIC_SDA_IN)
	{
		ack = 1;
	}
	else
	{
		ack = 0;
	}
	Delay_Us(us);
	
	
	IIC_SCL_L;
	Delay_Us(us);
	return ack;
}

/*************************
函数名称：IIc_Send_Byte(u8 data)
函数功能：IIc发送一个字节
返回值：无
形参：u8 data
作者：me
版本：1.0 
*************************/
void IIc_Send_Byte(u8 data)
{
	for(u8 i=0;i<8;i++)
	{
		IIC_SCL_L;
		if(data & (0x80 >> i))
		{
			IIC_SDA_H;	
		}
		else
		{
			IIC_SDA_L;	
		}
		Delay_Us(us);
		IIC_SCL_H;
		Delay_Us(us);
	}
	
}

/*************************
函数名称：IIc_Res_Byte(void)
函数功能：IIc接收一个字节
返回值：u8 
形参：无
作者：me
版本：1.0 
		数据是从高位开始发
*************************/
u8 IIc_Res_Byte(void)
{
	u8 data;
	IIC_SDA_H;     // ? 释放SDA，让传感器能驱动总线
	for(u8 i=0;i<8;i++)
	{
		
		IIC_SCL_L;//拉低时钟线，从机向主机发送1bit数据
		Delay_Us(us);
		IIC_SCL_H;//拉高时钟线，从机向主机发送1bit数据
		Delay_Us(us);
		
		data <<= 1;
		if(IIC_SDA_IN)
		{
			data |= 1;
		}
	}
	
	return data;
}
