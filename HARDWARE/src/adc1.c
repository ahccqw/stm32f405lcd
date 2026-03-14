#include "adc1.h"

u16 ADC1_Buf[2] = {0};
/*************************
函数名称：ADC1_Init(void)
函数功能：ADC1接收光敏及火焰
返回值：无
形参：无
作者：me
版本：1.0 
			光敏 PCO ----  ADC1_IN10		APB2	火焰 PC1	----  ADC1_IN11
*************************/
void ADC1_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
		
	ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div4;//adc 预分频为4 84 / 4  21M ,adc在407 405 上最快转换的速度为 36M
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	ADC_InitTypeDef ADC_InitStruct = {0};
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//连续转换
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//数据右对齐
	ADC_InitStruct.ADC_NbrOfConversion = 2;  // 转换次数为2
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;//分辨率为12位
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//用连续扫描
	
	ADC_Init(ADC1,&ADC_InitStruct);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_10,1,ADC_SampleTime_480Cycles);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_11,2,ADC_SampleTime_480Cycles);
	
	ADC_EOCOnEachRegularChannelCmd(ADC1,ENABLE);
	
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_SoftwareStartConv(ADC1);
}
