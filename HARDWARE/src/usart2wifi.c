#include "usart2wifi.h"

/*************************
函数名称：Usart2_Init(u32 baud)
函数功能：Usart2初始化函数
返回值:无
形参：u32 baud
作者：me
版本：1.0 
		U2 TX - PA2		U2	RX - PA3				APB1
*************************/
void Usart2_Init(u32 baud)
{
	//打开串口/IO时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//初始化IO口 复用模式	
	//初始化IO口 复用模式	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);//PD8服用到USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);//PD9服用到USART2
	
	//初始化USART2控制器
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = baud;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// 无硬件流控制	
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;		//无校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;		//一位停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;		//8位数据位
	USART_Init(USART2, &USART_InitStruct);
	
	//中断使能
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//接收中断，接收数据断，接受到一个字节触发
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//空闲中断，接收结束后经过1个帧还没有数据的时候为1
	
	//NVIC
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;//要配置的中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 4;//抢占优先级（值越小优先级越高）
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//次级优先级（值越小优先级越高）
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能，失能中断
	NVIC_Init( &NVIC_InitStruct);
	
	//U2 使能
	USART_Cmd(USART2,ENABLE);
}

U2 u2;
/*************************
函数名称：USART2_IRQHandler(void)
函数功能：USART2接收中断服务函数
返回值：无
形参：无
作者：me
版本：1.0 
函数说明:这里结构体由于是全局的变量，则初始值为0	
*************************/
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE))//如果接收到了数据	0中断未触发 1触发
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);//清除接收中断标志位
		u2.buff[u2.len++] = USART_ReceiveData(USART2);
		
	}
	
	if(USART_GetITStatus(USART2, USART_IT_IDLE))//进入空闲中断
	{
		
		u2.buff[u2.len] = '\0';
		USART2->SR;//清标志位
		USART2->DR;//清标志位
			
		u2.len = 0;   
		u2.flag = 1;
		if(strstr((char *)u2.buff,"MQTTSUBRECV") != NULL)
		{
			u2.rec_flag = 1;
		}

		printf("%s", u2.buff);  // 打印接收到ESP-12F的数据
		
	}
}

/*************************
函数名称：USART2_SendByte(u8 data)
函数功能：USART2字节发送函数
返回值:无
形参：u8 data
作者：me
版本：1.0 
*************************/
void USART2_SendByte(u8 data)
{
	//等待上一个字节发送完成 完成的时候为0 则需非
	while(!(USART_GetFlagStatus(USART2, USART_FLAG_TXE)));//TXE发送寄存器为空
	USART_SendData(USART2,data);
	//等待这个字节发送完成
	while(!(USART_GetFlagStatus(USART2, USART_FLAG_TC)));//TC发送完成
}

/*************************
函数名称：USART2_SendString(u32 data)
函数功能：USART2字符串发送函数
返回值:无
形参：u32 data
作者：me
版本：1.0 
*************************/
void USART2_SendString(char *str)
{	
	    while(*str != '\0')
    {
        USART2_SendByte(*str++);
    }
		
}

/*************************
函数名称：Esp_SendData(u8 *at,u16 timeout)
函数功能：发送AT指令并判断响应
返回值:1 代表AT指令失败， 0 代表 AT指令成功
形参:u8 *at,发送的指令
		 u16 timeout 超时计数
作者：me
版本：1.0 
*************************/
u8 Esp_SendData(u8 *at,u16 timeout)
{
	u16 count = 0;
	USART2_SendString((char *)at);
	Delay_Ms(100);
	
	while(1)
	{
		while(!u2.flag)
		{
			Delay_Ms(1);
			count++;
			if(count >= timeout)
			{
				u2.len = 0;//防止数据接到一半，没有接完，此时就返回0 重新开始接
				return 1;
			}
		}
		u2.flag = 0;
		//需要在u2.buff 中找 ok
		if(strstr((const char*)u2.buff,"OK") != NULL)
		{
			return 0;
		}			
	}
}


/*************************
函数名称：Wifi_Tcp_Init(void)
函数功能：TCP初始化
返回值:无
形参：void
作者：me
版本：1.0 
*************************/
void Wifi_Tcp_Init(void)
{
    u8 ret;
		printf("进入Wifi_Tcp_Init\r\n");  // 新增
    // 清屏全黑，显示初始化标题
    Lcd_Clear(0, 0, 240, 320, BLACK);
    Lcd_Display_Str(10, 35, WHITE, BLACK, 16, (u8 *)"WiFi连接中...");

    Usart2_Init(115200);
    //退出发送模式
    USART2_SendString("+++");
    Delay_Ms(100);
    //发送AT\r\n，跳过格式报错
    USART2_SendString("AT\r\n");
    Delay_Ms(100);

    ret = Esp_SendData((u8 *)"AT+MQTTCLEAN=0\r\n", 2000);
    if(ret == 0)
    {
        printf("断开服务器成功\r\n");
        Lcd_Display_Str(10, 65, GREEN, BLACK, 16, (u8 *)"1.断开旧服务器  成功");
    }
    else
    {
        Lcd_Display_Str(10, 65, RED, BLACK, 16, (u8 *)"1.断开旧服务器  失败");
    }

    //①设置WIFI模式为客户端模式		AT+CWMODE=1\r\n
    ret = Esp_SendData((u8 *)"AT+CWMODE=1\r\n", 5000);
    if(ret == 0)
    {
        printf("设置WIFI模式为客户端模式成功\r\n");
        Lcd_Display_Str(10, 90, GREEN, BLACK, 16, (u8 *)"2.设置客户端模式  成功");
    }
    else
    {
        Lcd_Display_Str(10, 90, RED, BLACK, 16, (u8 *)"2.设置客户端模式  失败");
    }

    //②连接软路由（连接WIFI）AT+CWJAP=\"Class8\",\"12345678\"\r\n
    ret = Esp_SendData((u8 *)"AT+CWJAP=\"ahccqw\",\"88888888\"\r\n", 10000);
    if(ret == 0)
    {
        printf("连接AP成功\r\n");
        Lcd_Display_Str(10, 115, GREEN, BLACK, 16, (u8 *)"3.连接WiFi  成功");
    }
    else
    {
        Lcd_Display_Str(10, 115, RED, BLACK, 16, (u8 *)"3.连接WiFi  失败");
    }

    //③设置传输模式为透传模式	AT+CIPMODE=1
    ret = Esp_SendData((u8 *)"AT+CIPMODE=1\r\n", 10000);
    if(ret == 0)
    {
        printf("设置为透传模式成功\r\n");
        Lcd_Display_Str(10, 140, GREEN, BLACK, 16, (u8 *)"4.设置透传模式  成功");
    }
    else
    {
        Lcd_Display_Str(10, 140, RED, BLACK, 16, (u8 *)"4.设置透传模式  失败");
    }

    //④设置MQTT用户链接参数
    ret = Esp_SendData((u8 *)"AT+MQTTUSERCFG=0,1,\"7ef12f698119426daa6289a1bdfd6042\",\"1ke8rhczmzyb8e9s\",\"ZBdO0RoYJ8\",0,0,\"\"\r\n", 10000);
    if(ret == 0)
    {
        printf("设置MQTT用户成功\r\n");
        Lcd_Display_Str(10, 165, GREEN, BLACK, 16, (u8 *)"5.设置MQTT用户  成功");
    }
    else
    {
        Lcd_Display_Str(10, 165, RED, BLACK, 16, (u8 *)"5.设置MQTT用户  失败");
    }

    //⑤连接服务器
    ret = Esp_SendData((u8 *)"AT+MQTTCONN=0,\"gz-3-mqtt.iot-api.com\",1883,1\r\n", 10000);
    if(ret == 0)
    {
        Lcd_Display_Str(10, 190, GREEN, BLACK, 16, (u8 *)"6.连接MQTT服务器  成功");
    }
    else
    {
        Lcd_Display_Str(10, 190, RED, BLACK, 16, (u8 *)"6.连接MQTT服务器  失败");
    }

    //⑥订阅下发属性
    ret = Esp_SendData((u8 *)"AT+MQTTSUB=0,\"attributes/push\",0\r\n", 1000);

    // 全部完成提示
    Lcd_Display_Str(10, 260, WHITE, BLACK, 16, (u8 *)"初始化完成!");
    Delay_Ms(1500);  // 停留1.5秒让用户看清结果
}


/*************************
函数名称：Wifi_Control(void)
函数功能：Wifi服务器控制 接受短消息
返回值:无
形参：void
作者：me
版本：1.0 
*************************/
void Wifi_Control(void)
{
    char *rgb_pos        = NULL;
    char *motor_pos      = NULL;
    char *serve_motor_pos= NULL;
 
    u32 rgb_value        = 0;
    u16 serve_motor_value= 0;
    u16 motor_value      = 0;
    u8  r, g, b;
 
    if(u2.rec_flag == 1)
    {
        u2.rec_flag = 0;
        u2.flag     = 0;
        u2_music_page4_handled = 0;   /* 新帧到来，重置音乐处理标志 */
 
        /* -------- LED -------- */
        if(strstr((char *)u2.buff, "\"Led\":1") != NULL)
        {
            LED1_ON; LED2_ON; LED3_ON;
            cst.led_state    = 1;
            cst.current_page = 1;
            cst.need_upload  = 1;
        }
        if(strstr((char *)u2.buff, "\"Led\":0") != NULL)
        {
            LED1_OFF; LED2_OFF; LED3_OFF;
            cst.led_state    = 0;
            cst.current_page = 1;
            cst.need_upload  = 1;
        }
 
        /* -------- 杀菌灯 -------- */
        if(strstr((char *)u2.buff, "\"Germicidal_lamp\":1") != NULL)
        {
            Sterilize_ON;
            cst.need_upload = 1;
        }
        if(strstr((char *)u2.buff, "\"Germicidal_lamp\":0") != NULL)
        {
            Sterilize_OFF;
            cst.need_upload = 1;
        }
 
        /* -------- RGB -------- */
        rgb_pos = strstr((char *)u2.buff, "\"Rgb\":");
        if(rgb_pos != NULL)
        {
            sscanf(rgb_pos + 6, "%u", &rgb_value);
            r = (rgb_value >> 16) & 0xFF;
            g = (rgb_value >> 8)  & 0xFF;
            b =  rgb_value        & 0xFF;
            Rgb_Control(r, g, b);
            cst.rgb_state   = 1;
            cst.rgb_value   = rgb_value;
            cst.need_upload = 1;
        }
        if(strstr((char *)u2.buff, "\"rgb_ct\":0") != NULL)
        {
            Rgb_Control(0, 0, 0);
            cst.rgb_state   = 0;
            cst.rgb_value   = 0;
            cst.need_upload = 1;
        }
 
        /* -------- 舵机 -------- */
        serve_motor_pos = strstr((char *)u2.buff, "\"servo_motor\":");
        if(serve_motor_pos != NULL)
        {
            sscanf(serve_motor_pos + 14, "%u", &serve_motor_value);
            Servo_motor_Control(serve_motor_value);
            cst.servo_state  = (serve_motor_value != 120) ? 1 : 0;
            cst.current_page = 3;
            cst.need_upload  = 1;
        }
 
        /* -------- 风扇 -------- */
        motor_pos = strstr((char *)u2.buff, "\"motor\":");
        if(motor_pos != NULL)
        {
            sscanf(motor_pos + 8, "%u", &motor_value);
            cst.motor_value      = motor_value;
            cst.motor_state      = (motor_value != 0) ? 1 : 0;
            cst.need_upload      = 1;
            TIM_SetCompare3(TIM3, motor_value);
            cst.last_motor_state = cst.motor_state - 1;   /* 强制触发刷新 */
            cst.current_page     = 2;
        }
        if(strstr((char *)u2.buff, "\"fan\":0") != NULL)
        {
            TIM_SetCompare3(TIM3, 0);
            cst.motor_state  = 0;
            cst.motor_value  = 0;
            cst.current_page = 2;
            cst.need_upload  = 1;
        }
 
        /*
         * -------- 音乐指令 --------
         * 规则：
         *   - 当前不在音乐页面（page4）：主循环直接处理，切换页面
         *   - 当前在音乐页面（page4）：Audio_MusicPlay() 正在阻塞，
         *     指令已由 SysTick 中断处理（u2_music_page4_handled=1），
         *     主循环跳过，避免重复执行
         */
        if(cst.current_page != 4)
        {
						
            /* 不在音乐页面，主循环正常处理 */
            if(strstr((char *)u2.buff, "\"music\":\"1\"") != NULL)
            {
                status_dev.PlayState = PLAY_PREVIOUS;
                music_start          = 1;
                cst.current_page     = 4;
            }
            if(strstr((char *)u2.buff, "\"music\":\"2\"") != NULL)
            {
                music_start          = 0;
                status_dev.PlayState = PLAY_STOP;
            }
            if(strstr((char *)u2.buff, "\"music\":\"3\"") != NULL)
            {
                music_start      = 1;
                cst.current_page = 4;
            }
            if(strstr((char *)u2.buff, "\"music\":\"4\"") != NULL)
            {
                status_dev.PlayState = PLAY_NEXT;
                music_start          = 1;
                cst.current_page     = 4;
            }
        }
				// 简单方案：如果当前不在音乐页面，肯定没有阻塞，直接解锁
				if (cst.current_page != 4)
				{
						u2_music_page4_handled = 0; 
				}
				// 进阶方案：如果在音乐页面，但播放状态已经结束（例如 STOP），也可以解锁
				else if (status_dev.PlayState == PLAY_STOP)
				{
						 u2_music_page4_handled = 0;
				}
        /* 在音乐页面时，中断已经处理，此处不重复执行 */
    }
}


/*************************
函数名称：Wifi_Send_Message(void)
函数功能：Wifi定时发送长数据(温湿度)到服务器
返回值:无
形参：void
作者：me
版本：1.0 
*************************/
void Wifi_Send_Message(void)
{
	u8 send_buff[50] = {0};
	u8 pub_buff[50] = {0};
	
	if(tphm_flag == 1)
	{
			tphm_flag = 0;
			tphm_Init();
		
			sprintf((char *)pub_buff,"{\"Room_Temp\":%.1f,\"humidity\":%.1f}",TH_Data,RH_Data);
			sprintf((char *)send_buff,"AT+MQTTPUBRAW=0,\"attributes\",%d,0,0\r\n",strlen((char *)pub_buff));
			
			Esp_SendData(send_buff,500);
			Esp_SendData(pub_buff,500);	
	}	
}

/*************************
函数名称：Status_Upload(void)
函数功能：检测状态变化并上传到平台，主循环中调用
返回值：无
形参：无
作者：me
版本：1.0
*************************/
void Status_Upload(void)
{
    static u32 last_upload_time = 0;
    u32 now = Get_Ms();

    // 限制频率（3秒）
    if((now - last_upload_time) < 3000) return;
    if(cst.need_upload == 0 && cst.first_upload == 1) return;

    last_upload_time = now;
    cst.need_upload = 0;

    char pub_buff[256] = {0};
    char send_buff[100] = {0};
    char temp[64];
    int first = 1;

    strcat(pub_buff, "{");

    /******************  首次：全量上传 ******************/
    if(cst.first_upload == 0)
    {
        sprintf(pub_buff,
            "{"
            "\"Led\":%d,"
            "\"motor\":%u,"
            "\"fan\":%d,"
            "\"servo_motor\":%d,"
            "\"Rgb\":%u,"
            "\"rgb_ct\":%d,"
            "\"Germicidal_lamp\":%d"
            "}",

            cst.led_state,

            cst.motor_value,
            cst.motor_value ? 1 : 0,

            cst.servo_state ? 30 : 120,

            cst.rgb_value,
            cst.rgb_value ? 1 : 0,

            cst.Germicidal_state
        );

        // ? 同步“上次状态”
        cst.last_led_upload = cst.led_state;
        cst.last_motor_upload = cst.motor_value;
        cst.last_servo_upload = cst.servo_state;
        cst.last_rgb_upload = cst.rgb_value;
        cst.last_rgb_upload_state = cst.rgb_state;
        cst.last_germicidal_upload = cst.Germicidal_state;

        cst.first_upload = 1;  // 标记已上传
    }
    else
    {
        /******************  增量上传 ******************/

        /***** LED *****/
        if(cst.led_state != cst.last_led_upload)
        {
            cst.last_led_upload = cst.led_state;

            sprintf(temp, "\"Led\":%d", cst.led_state);
            if(!first) strcat(pub_buff, ",");
            strcat(pub_buff, temp);
            first = 0;
        }

        /***** MOTOR（数值 + fan）*****/
        if(cst.motor_value != cst.last_motor_upload)
        {
            cst.last_motor_upload = cst.motor_value;

            sprintf(temp, "\"motor\":%u", cst.motor_value);
            if(!first) strcat(pub_buff, ",");
            strcat(pub_buff, temp);
            first = 0;

            sprintf(temp, "\"fan\":%d", cst.motor_value ? 1 : 0);
            strcat(pub_buff, ",");
            strcat(pub_buff, temp);
        }

        /***** SERVO *****/
        if(cst.servo_state != cst.last_servo_upload)
        {
            cst.last_servo_upload = cst.servo_state;

            sprintf(temp, "\"servo_motor\":%d", cst.servo_state ? 30 : 120);
            if(!first) strcat(pub_buff, ",");
            strcat(pub_buff, temp);
            first = 0;
        }

        /***** RGB（数值 + rgb_ct）*****/
        if(cst.rgb_value != cst.last_rgb_upload ||
           cst.rgb_state != cst.last_rgb_upload_state)
        {
            cst.last_rgb_upload = cst.rgb_value;
            cst.last_rgb_upload_state = cst.rgb_state;

            sprintf(temp, "\"Rgb\":%lu", cst.rgb_value);
            if(!first) strcat(pub_buff, ",");
            strcat(pub_buff, temp);
            first = 0;

            sprintf(temp, "\"rgb_ct\":%d", cst.rgb_value ? 1 : 0);
            strcat(pub_buff, ",");
            strcat(pub_buff, temp);
        }

        /***** 消毒灯 *****/
        if(cst.Germicidal_state != cst.last_germicidal_upload)
        {
            cst.last_germicidal_upload = cst.Germicidal_state;

            sprintf(temp, "\"Germicidal_lamp\":%d", cst.Germicidal_state);
            if(!first) strcat(pub_buff, ",");
            strcat(pub_buff, temp);
            first = 0;
        }

        strcat(pub_buff, "}");

        // ? 没变化不发
        if(strlen(pub_buff) <= 2)
            return;
    }

    /****************** ? MQTT发送 ******************/
    sprintf(send_buff,
        "AT+MQTTPUBRAW=0,\"attributes\",%d,0,0\r\n",
        strlen(pub_buff)
    );

    Esp_SendData(send_buff, 500);
    Esp_SendData(pub_buff, 500);
}












