#include "main.h"


int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//无次级优先级 抢占级范围 0-15
	
	Led_Init();
	Beep_Init();
	Key_Init();
	Beep_Start();
	Usart1_Init(256000);
	IIc_Init();
	Rgb_Init();
	Time6_Intrerpute(1500);
	Spi1_Init();
	LCD_Init();
	Rtc_Init();
	Rtc_WakeUp(1);
	

	u16 value[2] = {0};
	ADC1_Init();
	ADC1_DMA2_Stream0_CH0_Init(2,value);
	
	
	RTC_STRUCT rtc_alarma = {.weekday=0xff,.day=0xff,.hour=20,.min=18,.second=0};
	Rtc_AlarmA(rtc_alarma);

	


	u8 key;
		
	
	while(1)
	{	
		
		key = Key_Scan();
		
		

		
		
		
		if(key == 1)
		{
			AlarmA_flag = 0;
		}	
		
		if(AlarmA_flag == 1)
		{
				Rtc_AlarmBeep();
		}
		if(AlarmA_flag == 0)
		{
				GPIO_ResetBits(GPIOD,GPIO_Pin_15);
		}
		

		
		
	
	}	
}

