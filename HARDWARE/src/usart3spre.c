#include "usart3spre.h"


/*************************
函数名称：Usart3_Init(u32 baud)
函数功能：Usart3初始化函数
返回值:无
形参：u32 baud
作者：me
版本：1.0 
		U3 TX - PD8		U3	RX - PD9				APB1
*************************/
void Usart3_Init(u32 baud)
{
	//打开串口/IO时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	//初始化IO口 复用模式	
	//初始化IO口 复用模式	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3);//PD8服用到USART3
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3);//PD9服用到USART3
	
	//初始化USART3控制器
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = baud;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// 无硬件流控制	
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;		//无校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;		//一位停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;		//8位数据位
	USART_Init(USART3, &USART_InitStruct);
	
	//中断使能
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//接收中断，接收数据断，接受到一个字节触发
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);//空闲中断，接收结束后经过1个帧还没有数据的时候为1
	
	//NVIC
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;//要配置的中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;//抢占优先级（值越小优先级越高）
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//次级优先级（值越小优先级越高）
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能，失能中断
	NVIC_Init( &NVIC_InitStruct);
	
	//U3 使能
	USART_Cmd(USART3,ENABLE);
}

U3 u3;
/*************************
函数名称：USART3_IRQHandler(void)
函数功能：USART3接收中断服务函数
返回值：无
形参：无
作者：me
版本：1.0 
函数说明:这里结构体由于是全局的变量，则初始值为0	
*************************/
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE))//如果接收到了数据	0中断未触发 1触发
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);//清除接收中断标志位
		u3.buff[u3.len++] = USART_ReceiveData(USART3);
		
	}
	
	if(USART_GetITStatus(USART3, USART_IT_IDLE))//进入空闲中断
	{
		USART3->SR;//清标志位
		USART3->DR;//清标志位
		
		printf("%s", u3.buff);  // 打印接收到的数据
		u3.len = 0;
		u3.flag = 1;
		for(u8 i=0;i<3;i++)
		{
			printf("0x%02x  ", u3.buff[i]);
			
		}
		printf("\r\n");
		
	}
}


/*************************
函数名称：Voice_Control(void)
函数功能：语言控制函数
返回值：无
形参：无
作者：me
版本：1.0 
*************************/
void Voice_Control(void)
{
	//判断U3是否接收数据完成
	if(u3.flag == 1)
	{
		u3.flag = 0;
		switch(u3.buff[1])
		{
			case 0x35:
					//唤醒词	小凌小凌 	需要做什么
					cst.current_page = 0;  // 切换到电机页面显示
					Touch_Range(0,0,240,320);
					//播放语音反馈
					Audio_PlaySong((u8 *)"0:/voice/迎新.wav");
			break;
		
			case 0x01:
					//打开风扇		需要做什么
					cst.motor_state = 1;
					cst.current_page = 2;  // 切换到电机页面显示
					TIM_SetCompare3(TIM3,1000);
					Touch_Range(0,0,240,320);
					//语言反馈
					Audio_PlaySong((u8 *)"0:/voice/风扇开.wav");
					
			break;
			
			case 0x02:
					//关闭风扇
					cst.motor_state = 0;
					cst.current_page = 2;  // 切换到电机页面显示
					TIM_SetCompare3(TIM3,0);
					Touch_Range(0,0,240,320);
					//语言反馈
					Audio_PlaySong((u8 *)"0:/voice/风扇关.wav");
					break;
			case 0x10:
					//开灯
					cst.led_state = 1;
					cst.current_page = 1;  // 切换到LED页面显示
					LED1_ON; LED2_ON; LED3_ON;
					Rgb_Control(0xff,0xff,0xff);
					Rgb_Control(0xff,0xff,0xff);
					Rgb_Control(0xff,0xff,0xff);
					Rgb_Control(0xff,0xff,0xff);
					Touch_Range(0,0,240,320);
					//语言反馈
					Audio_PlaySong((u8 *)"0:/voice/灯全开.wav");
			break;
			
			case 0x11:
					//关灯
					cst.led_state = 0;
					cst.current_page = 1;  // 切换到LED页面显示
					LED1_OFF; LED2_OFF; LED3_OFF;
					Rgb_Control(0x00,0x00,0x00);
					Rgb_Control(0x00,0x00,0x00);
					Rgb_Control(0x00,0x00,0x00);
					Rgb_Control(0x00,0x00,0x00);
					Touch_Range(0,0,240,320);
					//语言反馈
					Audio_PlaySong((u8 *)"0:/voice/灯全关.wav");
			break;
			
			case 0x1D:
					//打开吹风		开舵机
					cst.servo_state = 1;
					cst.current_page = 3;  // 切换到舵机页面显示
					Servo_motor_Control(30);
					Touch_Range(0,0,240,320);
					//语言反馈
					Audio_PlaySong((u8 *)"0:/voice/吹风开.wav");
			break;
						
			case 0x1E:
					//关闭吹风	关闭舵机
					cst.servo_state = 0;
					cst.current_page = 3;  // 切换到舵机页面显示
					Servo_motor_Control(120);
					Touch_Range(0,0,240,320);
					//语言反馈
					Audio_PlaySong((u8 *)"0:/voice/吹风关.wav");
			break;
			
			case 0x0c:
					//升高温度		上一首歌	
					cst.rang_flag = 5;	
					cst.current_page = 4;  // 切换到音乐页面显示
					Touch_Range(0,0,240,320);
					//语言反馈
					Audio_PlaySong((u8 *)"0:/voice/切换上一首歌.wav");
			break;
			
			case 0x07:
					//打开空调		播放歌曲
					cst.rang_flag = 6;	
					cst.current_page = 4;  // 切换到音乐页面显示
					Touch_Range(0,0,240,320);
					//语言反馈
					Audio_PlaySong((u8 *)"0:/voice/播放歌曲.wav");
			break;
			
			case 0x08:
					//关闭空调		暂停歌曲
					cst.rang_flag = 6;	
					cst.current_page = 4;  // 切换到音乐页面显示
					Touch_Range(0,0,240,320);
					//语言反馈
					Audio_PlaySong((u8 *)"0:/voice/暂停.wav");
			break;
			
			case 0x0D:
					//降低温度		下一首歌
					cst.rang_flag = 7;	
					cst.current_page = 4;  // 切换到音乐页面显示
					Touch_Range(0,0,240,320);
					//语言反馈
					Audio_PlaySong((u8 *)"0:/voice/切换下一首歌.wav");
			break;			
		}
	}
	
}



