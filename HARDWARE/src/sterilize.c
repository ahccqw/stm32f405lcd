#include "sterilize.h"

/*************************
函数名称：Sterilize_Init(void)
函数功能：杀菌灯初始化函数 拉低电平
返回值:无
形参：无
作者：me
版本：1.0
	RELAY_PE0	
*************************/
void Sterilize_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_0);
}

