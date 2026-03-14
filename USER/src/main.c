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
//	SysTick_Intrerput_Init(1500);
	
	
//	Tim3_DcMotor_Init(0);
//	Tim2_ServoMotor_Init(500);
	u16 value[2] = {0};
	ADC1_Init();
	ADC1_DMA2_Stream0_CH0_Init(2,value);
	

	
//	if(KEY1_ON)//先长按复位按钮，再长按KEY1，接下来只需松开复位
//	{
//		printf("Font_Updata --- 1\r\n");
//		Font_Updata();
//	}
	
	Lcd_Display_Chinese_Char(20,70,BLUE,WHITE,16,(u8 *)"王");
	Lcd_Display_Chinese_Char(20,86,BLUE,WHITE,24,(u8 *)"仲");
	Lcd_Display_Chinese_Char(20,110,BLUE,WHITE,32,(u8 *)"钦");
	
	
	Lcd_Display_Char(80,70,BLUE,WHITE,16,'A');
	Lcd_Display_Char(80,86,BLUE,WHITE,24,'B');
	Lcd_Display_Char(80,110,BLUE,WHITE,32,'C');

//	Lcd_Display_Str(80,160,BLUE,WHITE,16,(u8 *)"AAA建材王总");
	
	//TIM_SetCompare3(TIM3, 600);
	

	
	
	

	
	
	
	
	
//	Lcd_Display_Photo(0,0,(u8 *)gImage_car4);
	
	
//	Lcd_Display_Char(50,70,BLUE,WHITE,16,A);
//	Lcd_Display_Char(50,86,BLUE,WHITE,24,B);
//	Lcd_Display_Char(50,110,BLUE,WHITE,32,C);
	
//	Lcd_Display_Chinese_Char(50,70,BLUE,WHITE,16,you);
//	Lcd_Display_Chinese_Char(50,86,BLUE,WHITE,24,hello);
//	Lcd_Display_Chinese_Char(50,110,BLUE,WHITE,32,good);
	
//	float temp;
	
	
//		u8 tp_buff[] = {0};
//		u8 hm_buff[] = {0};

		u8 time[32] = {0};
		u8 date[64] = {0};
	
		
	
	while(1)
	{	
		
		Rtc_GetValue();
		
		sprintf((char *)time,"time: %02d:%02d:%02d", rtc.hour, rtc.min, rtc.second);	
		sprintf((char *)date,"data: 20%02d-%02d-%02d Week:%d", rtc.year, rtc.month, rtc.day, rtc.weekday);		
		Lcd_Display_Str(5,140,BLUE,WHITE,32,time);
		Lcd_Display_Str(5,172,BLUE,WHITE,32,date);	
		Delay_Ms(1000);
		
		
//		Rtc_GetValue();
//		Delay_Ms(2000);
		
//		tphm_Init();
//		
//		sprintf((char *)tp_buff,"温度%.1f%%",TH_Data);
//		sprintf((char *)hm_buff,"湿度%.1f%%",RH_Data);
//		
//		Lcd_Display_Str(80,160,BLUE,WHITE,16,tp_buff);
//		Lcd_Display_Str(80,200,BLUE,WHITE,16,hm_buff);
//		Delay_Ms(500);
	
		
		
		
//		temp = infrared_tp();
//		printf("infrared_tp:%.2f\r\n",temp);
//		Delay_Ms(2000);
		

//		tphm_Init();
//		printf("RH:%d   TP:%d\r\n",RH_Data,TH_Data);
//		Delay_Ms(2000);
//		
		
		
		
//		Rgb_Control(32,178,170);
//		Rgb_Control(124,252,0);
//		Rgb_Control(255,255,0);
//		Rgb_Control(255,0,0);
//		Rgb_Rest();
		
//		Rgb_Rainbow_Cycle(15,360);
		
		
	}	
}

