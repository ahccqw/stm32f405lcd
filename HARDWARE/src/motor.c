#include "motor.h"

/*************************
函数名称：Tim3_DcMotor(void)
函数功能：Tim3_DcMotor(void) 
返回值：无
形参：无
作者：me
版本：1.0 
PC8 - time3_CH3 APB1 
*************************/
void Tim3_DcMotor_Init(u16 ccr)
{
	//时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	//IO口复用
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;  // 新增
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;      // 新增
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;  // 新增
	GPIO_Init(GPIOC,&GPIO_InitStruct);

	//时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//计数模式，向上计数
	TIM_TimeBaseInitStruct.TIM_Period = 1000 -1;//重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84 -1;//预分频 总线频率为84与AHB1频率不同则需 * 2 ,为84
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
	
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;//PWM模式1
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//输出状态（使能）
	TIM_OCInitStruct.TIM_Pulse = ccr;//脉冲（占空比）
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性 有效电平
	TIM_OC3Init(TIM3, &TIM_OCInitStruct);
	
	//打开缓存
	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_Cmd(TIM3, ENABLE);
}



