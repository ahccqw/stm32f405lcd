#include "key.h"

/*************************
函数名称：Key_Init(void)
函数功能：Key初始化函数 
返回值:无
形参：无
作者：me
版本：1.0 
KEY1 -- H_ON -- PA0		KEY2 -- L_ON -- PB0		KEY3 -- L_ON -- PB1
*************************/
void Key_Init(void)
{
	//打开时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB,ENABLE);
	//IO初始化
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}

/*************************
函数名称：Key_Scan(void)
函数功能：Key扫描函数 
返回值:u8 
形参：无
作者：me
版本：1.0 
*************************/
u8 Key_Scan(void)
{
	static u8 key_flag = 0;
	if((KEY1_ON || KEY2_ON || KEY3_ON) && !key_flag)
	{
		Delay_Ms(30);
		key_flag = 1;
		
		if(KEY1_ON)
		{
			return 1;
		}
		
		if(KEY2_ON)
		{
			return 2;
		}
		
		if(KEY3_ON)
		{
			return 3;
		}
	}
	else if((!KEY1_ON && !KEY2_ON && !KEY3_ON) && key_flag)
	{
		key_flag = 0;	

	}
	return 0xff;
}


