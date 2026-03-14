#include "spi1.h"

/*************************
函数名称：Spi1_Init(void)
函数功能：Spi1初始化配置函数
返回值：无
形参：无
作者：me
版本：1.0 
	传输配置为模式0或模式3
	SPI1_MOSI		---		PA7		---		复用	AF5 (0-7)
	SPI1_MISO		---		PA6		---		复用
	SPI1_SCK		---		PA5		---		复用
				CS		---		PC7	---		通用推挽输出
*************************/
void Spi1_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); // 使能SPI1时钟
	//IO口初始化
		
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT; 
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//SPI1初始化
	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//波特率预分频
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;//第一个边沿采样
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;//时钟极性空闲时为低电平
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//8位数据帧
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//全双工
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//MSB先发
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master; //主机模式
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;//软件控制，CS由IO控制
	SPI_Init(SPI1,&SPI_InitStruct);
	
	SPI_Cmd(SPI1,ENABLE);
}

u8 Spi1_Change_By(u8 data)
{

	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1,data);
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));	
	return SPI_I2S_ReceiveData(SPI1);
}

