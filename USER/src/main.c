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
//	Rtc_Init();
//	Rtc_WakeUp(1);
	
	Touch_Init();

	u16 value[2] = {0};
	ADC1_Init();
	ADC1_DMA2_Stream0_CH0_Init(2,value);
	
	
//	RTC_STRUCT rtc_alarma = {.weekday=0xff,.day=0xff,.hour=20,.min=18,.second=0};
//	Rtc_AlarmA(rtc_alarma);

//		while(SD_Initialize())
//	{
//			printf("请插入SD卡\r\n");
//	}
//	printf("SD卡初始化成功\r\n");
//	
//	u8 buf[10] = {'1','2','3','4'};
//	u8 test[10] = {0};
//	
//	u32 sector_count = 0;
//	float sd_size = 0;
//	sector_count = SD_GetSectorCount();
//	sd_size = sector_count * 512.0f / 1024.0f / 1024.0f / 1024.0f;
//	printf("SD_SIZE: %.2fGB\r\n",sd_size);
//	
//	SD_WriteDisk(buf,0,1);
//	SD_ReadDisk(test,0,1);
//	
//	printf("test: %s\r\n",test);
	
//	u8 key;
		
	
	while(1)
	{	
		
		Touch_Coordinates();

		Touch_Range(0,0,240,320);
		Delay_Ms(50);
	
//		
//		Delay_Ms(300);
		
		
		
		
		
		
		
		
		
//		
//		key = Key_Scan();
//		
//		

//		
//		
//		
//		if(key == 1)
//		{
//			AlarmA_flag = 0;
//		}	
//		
//		if(AlarmA_flag == 1)
//		{
//				Rtc_AlarmBeep();
//		}
//		if(AlarmA_flag == 0)
//		{
//				GPIO_ResetBits(GPIOD,GPIO_Pin_15);
//		}
		

		
		
	
	}	
}

