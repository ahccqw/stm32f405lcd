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
	Tim2_ServoMotor_Init(0);
	Usart3_Init(115200);
	Tim3_DcMotor_Init(0);
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
	UINT bw,br;

	WM8978_Init();				//初始化WM8978
	WM8978_HPvol_Set(0, 0);		//耳机音量设置
	WM8978_SPKvol_Set(0);		//喇叭音量设置
	WM8978_ADDA_Cfg(1, 0);		//开启DAC
	WM8978_Input_Cfg(0, 0, 0);	//关闭输入通道
	WM8978_Output_Cfg(1, 0);	//开启DAC输出   
	WM8978_I2S_Cfg(2, 0);		//飞利浦标准,16位数据长度
	I2S2_Init(I2S_Standard_Phillips, I2S_Mode_MasterTx, I2S_CPOL_Low, I2S_DataFormat_16bextended);	//飞利浦标准,主机发送,时钟低电平有效,16位扩展帧长度
	I2S2_SampleRate_Set(44100);	//设置采样率
	I2S2_TX_DMA_Init(NULL, NULL, WAV_I2S_TX_DMA_BUFSIZE/2); 				//配置TX DMA
	status_dev.volume = 60;	//初始保存音量 0~63
	WM8978_SPKvol_Set(status_dev.volume);
	

	ret = f_mount (&fs,"0",1);
	if(ret == FR_OK)
	{
		printf("卡片注册成功\r\n");			
	}
	
	else
	{
		printf("卡片注册失败\r\n");	
	}
		

//	Audio_PlaySong((u8 *)"0:/music/晴天.wav");

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

	
//	u8 key;
		
	
	while(1)
	{	
		
		Voice_Control();
		
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

