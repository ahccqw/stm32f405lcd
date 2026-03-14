#include "systick.h"


/*************************
函数名称：SysTick_Intrerput(u16 nms)
函数功能：SysTick_Intrerput(u16 nms)初始化
返回值：无
形参：u16 nms
作者：me
版本：1.0
函数说明：参考时钟源，21mhz
*************************/
void SysTick_Intrerput_Init(u16 nms)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	
	SysTick->LOAD = nms * 21000 -1;;
	
	// 清除当前计数值
	SysTick->VAL = 0;
	
	// 使能SysTick中断，使能计数器
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	
	NVIC_SetPriority(TIM7_IRQn,NVIC_EncodePriority (0,8,0));//设置优先级
	
	// 注意：不需要调用 NVIC_EnableIRQ(SysTick_IRQn)，因为 SysTick 的中断使能
  // 是由 SysTick->CTRL 的 TICKINT 位控制的，而不是 NVIC 的 ISER 寄存器。
//	NVIC_InitTypeDef NVIC_InitStruct;
//	NVIC_InitStruct.NVIC_IRQChannel = SysTick_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 8;
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
//	NVIC_Init(&NVIC_InitStruct);

}

/*************************
函数名称：SysTick_Handler(void)
函数功能：定时中断处理函数
返回值：无
形参：无
作者：me
版本：1.0
函数说明：参考时钟源，21mhz
*************************/
void SysTick_Handler(void)
{
	  static u32 tick = 0;
    tick++;
    printf("systick tick: %d\n", tick); // 打印当前tick计数，用于调试
}


