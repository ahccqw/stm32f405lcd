#include "led.h"

/*************************
函数名称：Led_Init(void)
函数功能：Led初始化函数 且拉高电平
返回值:无
形参：无
作者：me
版本：1.0 
 PD 12 13 14 低电平亮
*************************/
void Led_Init(void)
{
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//打开GPIOD
	
	//结构体变量的定义
	GPIO_InitTypeDef GPIO_InitStruct={0};	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上拉下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	//初始电平
	GPIO_SetBits(GPIOD,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
	
}
