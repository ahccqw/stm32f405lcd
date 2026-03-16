#include "touch.h"

/*************************
函数名称：Touch_IIc_Init(void)
函数功能：IIc触摸初始化配置函数
返回值：无
形参：无
作者：me
版本：1.0 
	IIC3_SCL --- PA8 --- 通用推挽输出
	IIC3_SDA --- PC9 --- 通用开漏输出				AHB1	
	TP_RST	 --- PD5 --- 通用推挽输出				复位输入低电平有效
	TP_TINT	 --- PD1 --- 输入模式						
																					设备地址为：0X15,写入地址为0X2A,读地址为0X2B
*************************/
void Touch_IIc_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOD,&GPIO_InitStruct);

	TOUCH_IIC_SCL_H;
	TOUCH_IIC_SDA_H;
	TP_RST_H;
}

/*************************
函数名称：Touch_IIc_Start(void)
函数功能：Touch_IIc起始信号函数 
返回值：无
形参：无
作者：me
版本：1.0 
	在时钟线为高电平的时候去拉低数据线
*************************/
void Touch_IIc_Start(void)
{
	TOUCH_IIC_SDA_H;
	Delay_Us(us);
	
	TOUCH_IIC_SCL_H;
	Delay_Us(us);
	
	TOUCH_IIC_SDA_L;
	Delay_Us(us);
	
	
}

/*************************
函数名称：Touch_IIc_End(void)
函数功能：Touch_IIc结束信号函数 
返回值：无
形参：无
作者：me
版本：1.0 
	在时钟线为高电平的时候去拉高数据线
*************************/
void Touch_IIc_End(void)
{
	TOUCH_IIC_SDA_L;
	Delay_Us(us);
	
	TOUCH_IIC_SCL_H;
	Delay_Us(us);
	
	TOUCH_IIC_SDA_H;
	Delay_Us(us);
}

/*************************
函数名称：Touch_IIc_Send_Ack(u8 ack)
函数功能：Touch_IIc发送应答/不应答信号函数 
返回值：无
形参：u8 ack		0 发送应答 	1 发送不应答
作者：me
版本：1.0 
*************************/
void Touch_IIc_Send_Ack(u8 ack)
{
	TOUCH_IIC_SCL_L;
	Delay_Us(us);
	if(ack == 1)
	{
		TOUCH_IIC_SDA_H;
	}
	else
	{
		TOUCH_IIC_SDA_L;
	}
	Delay_Us(us);
	
	TOUCH_IIC_SCL_H;
	Delay_Us(us);
	
	TOUCH_IIC_SCL_L;
	Delay_Us(us);
}

/*************************
函数名称：Touch_IIc_Rec_Ack(void)
函数功能：Touch_IIc接收应答/不应答信号函数 
返回值：u8 ack		0 表示应答 	1 表示不应答
形参：无
作者：me
版本：1.0 				SCL推挽		SDA开漏
*************************/
u8 Touch_IIc_Rec_Ack(void)
{
	u8 ack;
	
	TOUCH_IIC_SDA_H;
	
	TOUCH_IIC_SCL_L;//拉低时钟线，从机向主机发送1bit应答
	Delay_Us(us);
	TOUCH_IIC_SCL_H;//拉高时钟线，主机接收刚刚从机发送的1bit应答
	Delay_Us(us);
	if(TOUCH_IIC_SDA_IN)
	{
		ack = 1;
	}
	else
	{
		ack = 0;
	}
	Delay_Us(us);
	
	
	TOUCH_IIC_SCL_L;
	Delay_Us(us);
	return ack;
}

/*************************
函数名称：Touch_IIc_Send_Byte(u8 data)
函数功能：Touch_IIc发送一个字节
返回值：无
形参：u8 data
作者：me
版本：1.0 
*************************/
void Touch_IIc_Send_Byte(u8 data)
{
	for(u8 i=0;i<8;i++)
	{
		TOUCH_IIC_SCL_L;
		if(data & (0x80 >> i))
		{
			TOUCH_IIC_SDA_H;	
		}
		else
		{
			TOUCH_IIC_SDA_L;	
		}
		Delay_Us(us);
		TOUCH_IIC_SCL_H;
		Delay_Us(us);
	}
	
}

/*************************
函数名称：Touch_IIc_Res_Byte(void)
函数功能：Touch_IIc接收一个字节
返回值：u8 
形参：无
作者：me
版本：1.0 
		数据是从高位开始发
*************************/
u8 Touch_IIc_Res_Byte(void)
{
	u8 data;
	IIC_SDA_H;     // ? 释放SDA，让传感器能驱动总线
	for(u8 i=0;i<8;i++)
	{
		
		TOUCH_IIC_SCL_L;//拉低时钟线，从机向主机发送1bit数据
		Delay_Us(us);
		TOUCH_IIC_SCL_H;//拉高时钟线，从机向主机发送1bit数据
		Delay_Us(us);
		
		data <<= 1;
		if(TOUCH_IIC_SDA_IN)
		{
			data |= 1;
		}
	}
	
	return data;
}

/*************************
函数名称：Touch_Init(void)
函数功能：Touch初始化
返回值：无 
形参：无
作者：me
版本：1.0 
*************************/
void Touch_Init(void)
{
	u8 id=0;
	Touch_IIc_Init();
	TP_RST_L;
	Delay_Ms(50);
	TP_RST_H;
	Delay_Ms(50);
	Touch_Wr_Byte(0xFD,0);
	Touch_IIc_Re_Ct_Byte(0xA7,&id,1);
	printf("ChipID:0x%02x\r\n",id);

}


/*************************
函数名称：Touch_Wr_Byte(void)
函数功能：IIc写入一个字节
返回值：无 
形参：无
作者：me
版本：1.0 
		数据是从高位开始发
*************************/
void Touch_Wr_Byte(u8 addr,u8 data)
{
	Touch_IIc_Start();
	Touch_IIc_Send_Byte(0X2A);
	Touch_IIc_Rec_Ack();
	Touch_IIc_Send_Byte(addr);
	Touch_IIc_Rec_Ack();
	Touch_IIc_Send_Byte(data);
	Touch_IIc_Rec_Ack();
	Touch_IIc_End();
}

/*************************
函数名称：Touch_IIc_Res_Byte(u8 addr,u8 *data,u32 len)
函数功能：IIc读取多个字节
返回值：无 
形参：u8 addr,u8 *data,u32 len
作者：me
版本：1.0 
		数据是从高位开始发
*************************/
void Touch_IIc_Re_Ct_Byte(u8 addr,u8 *data,u32 len)
{
	Touch_IIc_Start();
	Touch_IIc_Send_Byte(0X2A);
	Touch_IIc_Rec_Ack();
	Touch_IIc_Send_Byte(addr);
	Touch_IIc_Rec_Ack();
	Touch_IIc_Start();
	Touch_IIc_Send_Byte(0X2B);
	Touch_IIc_Rec_Ack();
	while(len--)
	{
		*data = Touch_IIc_Res_Byte();
		data++;
	}
	Touch_IIc_End();
}
















