#include "time6.h"

u8 tphm_flag;
u32 cnt1;
/*************************
函数名称：Time6_Intrerpute(u16 nms)
函数功能：TIM6 更新中断配置
返回值：无
形参：nms  定时中断周期（单位：ms）
作者：me
版本：1.0
函数说明：
TIM6属于APB1总线定时器
通过设置预分频和自动重装值产生定时中断

定时计算：
APB1 Timer Clock = 84MHz
Prescaler = 8400 - 1 → 84MHz / 8400 = 10kHz (0.1ms)
Period = 10 * nms - 1

最终定时周期：
0.1ms × 10 × nms = nms ms
*************************/
void Time6_Intrerpute(u16 nms)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	
	// 定时器与中断结构体定义
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	// 定时器基本参数配置
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 10 * nms -1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 8400 -1;
	// 初始化TIM6
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);
	
	//中断使能
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	
	// 4. 配置NVIC中断控制器
	NVIC_InitStruct.NVIC_IRQChannel = TIM6_DAC_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 9;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
	
	// 5. 启动定时器
	TIM_Cmd(TIM6,ENABLE);
}

void TIM6_DAC_IRQHandler(void)
{
	//清除中断标志位
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
	
	cnt1++;
	if(cnt1 >= 1800)
	{
		cnt1 = 0;
		tphm_flag = 1;
	}
	
	
	

}
