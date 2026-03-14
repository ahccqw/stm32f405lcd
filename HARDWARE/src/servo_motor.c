#include "servo_motor.h"

/*************************
函数名称：Tim2_ServoMotor(u16 ccr)
函数功能：舵机初始化
返回值：无
形参：无
作者：me
版本：1.0 
PA1 - time2_CH2 APB1 42Mhz 
*************************/
void Tim2_ServoMotor_Init(u16 ccr)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1, GPIO_AF_TIM2);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 20000 -1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84 -1;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	
	TIM_OCInitTypeDef TIM_OCInitStruc;
	TIM_OCInitStruc.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruc.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruc.TIM_OCPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStruc.TIM_Pulse = 0;
	TIM_OC2Init(TIM2,&TIM_OCInitStruc);
	
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
	TIM_Cmd(TIM2, ENABLE);
}

#define	MAX_CCR 2500
#define MIN_CCR 500
#define MAX_ANGLE 180
#define PER_ANGLE ((MAX_CCR - MIN_CCR) / MAX_ANGLE)
void Servo_motor_Control(u8 angle)
{
	TIM_SetCompare2(TIM2,500 + angle * PER_ANGLE);
}

