#include "beep.h"

/*************************
函数名称：Beep_Init(void)
函数功能：Beep初始化函数 且拉低电平	PD15
返回值:无
形参：无
作者：me
版本：1.0 
*************************/
void Beep_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOD, GPIO_Pin_15);
}

/*************************
函数名称：Beep_Start(void)
函数功能：Beep开始响函数 且拉低电平	PD15
返回值:u8
形参：无
作者：me
版本：1.0 
*************************/
void Beep_Start(void)
{
	GPIO_SetBits(GPIOD, GPIO_Pin_15);
	Delay_Ms(100);
	GPIO_ResetBits(GPIOD, GPIO_Pin_15);
	Delay_Ms(100);
}

