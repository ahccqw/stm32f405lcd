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
	Tim2_ServoMotor_Init(0);
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

	
	u8 ret = 0;
	FATFS fs;
	FIL fp;
	UINT bw;
	UINT br;
	u8 write_buff[] = {"hello,world"};
	u8 temp[20];
	
	
	ret = f_mount (&fs,"0",1);
	if(ret == FR_OK)
	{
		printf("卡片注册成功\r\n");	
		ret = f_open (&fp,"0:/123.txt",FA_READ | FA_WRITE);
		if(ret == FR_OK)
		{
			printf("打开文件成功\r\n");
			
			ret = f_write (&fp,write_buff,sizeof(write_buff),&bw);			
			if(ret == FR_OK)
			{
				printf("写入成功,写入数据大小为: %d\r\n", bw);
        
        f_lseek(&fp, 0);  // 定位到文件开头    
        ret = f_read(&fp, temp, sizeof(write_buff), &br);
				
         if(ret == FR_OK)
				{
						printf("读取成功,读取数据大小为: %d\r\n", br);
						printf("写入内容为：%s\r\n", temp);
				}
				else
				{
						printf("读取失败，错误码: %d\r\n", ret);
				}		
				
			}
			 f_close(&fp);  // 只关闭一次
			
		}
		else
    {
        printf("打开文件失败，错误码: %d\r\n", ret);
    }
		
		
		
		
		if(ret == FR_OK)
		{		
			ret = f_open (&fp,"0:/123.txt",FA_READ | FA_WRITE);
			if(ret == FR_OK)
			{
				f_read (&fp,temp,sizeof(write_buff),&br);
				if(ret == FR_OK)
				{
					f_close (&fp);
					printf("读取成功,读取数据大小为: %d\r\n",br);
					printf("写入数据位：%s\r\n",temp);
					
				}
			}
		}
					
	}

	
	else
	{
		printf("卡片注册失败\r\n");	
	}
		



	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

	
//	u8 key;
		
	
	while(1)
	{	
		
		Touch_Coordinates();

		Touch_Range(0,0,240,320);
		
		
//		Delay_Ms(50);
	
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

