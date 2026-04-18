#include "main.h"
// 在 main.c 定义
WorkMode current_mode = MODE_SLAVE; // 默认主机

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
	Time6_Intrerpute(1000);	
	W25q64_Init();
	LCD_Init();
	Tim2_ServoMotor_Init(0);	
	Tim3_DcMotor_Init(0);
	Usart3_Init(115200);	
	
	//Wifi_Tcp_Init();



	Sterilize_Init();
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
	status_dev.volume = 63;	//初始保存音量 0~63
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
	SysTick_Intrerput_Init(1);	


	
//	Audio_PlaySong((u8 *)"0:/music/晴天.wav");

	//串口测试：
	u8 key,cnt,Mode,i,time;
	Can_Config(6,7,Mode,6);
	u8 send_buff[8];
	u8 rev_buff[8];
	
	
	//modbus  串口初始化
	if (current_mode == MODE_MASTER)
	{
		U4_Modbus_Init();
	}
	else if(current_mode == MODE_SLAVE)
	{
		U4_Modbus_Slave_Init();
	}

	modbus_master.rxcount = 0;
	modbus_master.rxover = 0;
	modbus_slve.rxcount = 0;
	modbus_slve.rxover = 0;
	
  	TIM7_7ms_Config();        // Modbus 断帧定时器 (必须保留，用于判断一帧结束)
	
   while(1)
  {
		
		
		// ================= 主机模式 =================
        if (current_mode == MODE_MASTER)
        {
            // --- 步骤 1: 发送请求 ---
            printf("主机发送请求...\r\n");
            ModbusMaster_Transmit();
            
            // --- 步骤 2: 等待从机回复 (阻塞延时) ---
            // 既然没有系统心跳，我们直接卡在这里等 100ms
            // 这段时间内，串口中断依然在后台工作，会把数据收进缓冲区
            Delay_Ms(100); 
            
            // --- 步骤 3: 检查是否收到数据 ---
            if (modbus_master.rxover == 1)
            {
                printf("主机收到回复!\r\n");
                ModbusMaster_Receive(); // 解析数据
                modbus_master.rxover = 0; // 清除标志
            }
            else
            {
                // 没收到数据，可能是从机没响应或接线错误
                printf("超时：未收到回复\r\n");
            }

            // --- 步骤 4: 等待下一次发送 (阻塞延时) ---
            // 这里卡住 2000ms (2秒)，实现循环发送的间隔
            Delay_Ms(2000);
        }
        
        // ================= 从机模式 =================
        else if (current_mode == MODE_SLAVE)
        {
            // 从机时刻监听总线
            if (modbus_slve.rxover == 1)
            {

								printf("收到数据开始解析\r\n");

								get_Value();
                // 收到数据，解析并回复
                modbus_slave_analy(); 
                
                // 清除标志
                modbus_slve.rxover = 0;
                modbus_slve.rxcount = 0;
            }
            
            // 从机循环里尽量少加延时，防止漏掉主机指令
            // 如果必须要加，只能加非常短的时间，比如 Delay_Us(100);
        }    
        
	
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		/*
		//按键一按下是发送信息 ， 按键二是切换模式
		key = Key_Scan();
		switch(key)
		{
			case 1:
			{
				Mode=!Mode;
				Can_Config(CAN_BS1_7tq,CAN_BS2_6tq,Mode,6);
				if(Mode==0)
				{
					printf("此时为普通模式\r\n");
				}
				else
				{
					printf("此时为环回模式\r\n");
				}
			}
			;break;
		
			case 2:
			{
				for(i=0;i<8;i++)
				{
					send_buff[i]=cnt+i;
				}
				printf("此时发送的数据如下:");
				for(i=0;i<8;i++)
				{
					printf("%d ",send_buff[i]);
				}
					printf("\r\n");
					Can_Send_Message(send_buff,sizeof(send_buff));
				}
			;break;	
				
			default: ;break;
		}
		
		key = Can_Res_Message(rev_buff);
		if(key)
		{
			printf("此时接收到的数据如下:");
			for(i=0;i<8;i++)
			{
				printf("%d ",rev_buff[i]);
			}
			printf("\r\n");
		}
		
			
		if(time == 50)
		{
			time = 0;
			printf("cnt=%d\r\n",cnt);
			cnt ++;
		}
		
		time ++;
		Delay_Ms(10);
		*/
		
		

		
//      Voice_Control();
//      Wifi_Control();
//      Touch_Coordinates();
//      Touch_Range(0, 0, 240, 320);
//      Wifi_Send_Message();
//      Status_Upload();
//      music_ct();   // 放最后，阻塞时中断仍处理触摸
		
		
  }
 
 
}

