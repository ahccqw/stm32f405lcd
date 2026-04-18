#include "usart2wifi.h"

/*************************
�������ƣ�Usart2_Init(u32 baud)
�������ܣ�Usart2��ʼ������
����ֵ:��
�βΣ�u32 baud
���ߣ�me
�汾��1.0 
		U2 TX - PA2		U2	RX - PA3				APB1
*************************/
void Usart2_Init(u32 baud)
{
	//�򿪴���/IOʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//��ʼ��IO�� ����ģʽ	
	//��ʼ��IO�� ����ģʽ	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);//PD8���õ�USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);//PD9���õ�USART2
	
	//��ʼ��USART2������
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = baud;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// ��Ӳ��������	
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;		//��У��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;		//һλֹͣλ
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;		//8λ����λ
	USART_Init(USART2, &USART_InitStruct);
	
	//�ж�ʹ��
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//�����жϣ��������ݶϣ����ܵ�һ���ֽڴ���
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//�����жϣ����ս����󾭹�1��֡��û�����ݵ�ʱ��Ϊ1
	
	//NVIC
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;//Ҫ���õ��ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 4;//��ռ���ȼ���ֵԽС���ȼ�Խ�ߣ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//�μ����ȼ���ֵԽС���ȼ�Խ�ߣ�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//ʹ�ܣ�ʧ���ж�
	NVIC_Init( &NVIC_InitStruct);
	
	//U2 ʹ��
	USART_Cmd(USART2,ENABLE);
}

U2 u2;
/*************************
�������ƣ�USART2_IRQHandler(void)
�������ܣ�USART2�����жϷ�����
����ֵ����
�βΣ���
���ߣ�me
�汾��1.0 
����˵��:����ṹ��������ȫ�ֵı��������ʼֵΪ0	
*************************/
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE))//������յ�������	0�ж�δ���� 1����
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);//��������жϱ�־λ
		u2.buff[u2.len++] = USART_ReceiveData(USART2);
		
	}
	
	if(USART_GetITStatus(USART2, USART_IT_IDLE))//��������ж�
	{
		
		u2.buff[u2.len] = '\0';
		USART2->SR;//���־λ
		USART2->DR;//���־λ
			
		u2.len = 0;   
		u2.flag = 1;
		if(strstr((char *)u2.buff,"MQTTSUBRECV") != NULL)
		{
			u2.rec_flag = 1;
		}

		printf("%s", u2.buff);  // ��ӡ���յ�ESP-12F������
		
	}
}

/*************************
�������ƣ�USART2_SendByte(u8 data)
�������ܣ�USART2�ֽڷ��ͺ���
����ֵ:��
�βΣ�u8 data
���ߣ�me
�汾��1.0 
*************************/
void USART2_SendByte(u8 data)
{
	//�ȴ���һ���ֽڷ������ ��ɵ�ʱ��Ϊ0 �����
	while(!(USART_GetFlagStatus(USART2, USART_FLAG_TXE)));//TXE���ͼĴ���Ϊ��
	USART_SendData(USART2,data);
	//�ȴ�����ֽڷ������
	while(!(USART_GetFlagStatus(USART2, USART_FLAG_TC)));//TC�������
}

/*************************
�������ƣ�USART2_SendString(u32 data)
�������ܣ�USART2�ַ������ͺ���
����ֵ:��
�βΣ�u32 data
���ߣ�me
�汾��1.0 
*************************/
void USART2_SendString(char *str)
{	
	    while(*str != '\0')
    {
        USART2_SendByte(*str++);
    }
		
}

/*************************
�������ƣ�Esp_SendData(u8 *at,u16 timeout)
�������ܣ�����ATָ��ж���Ӧ
����ֵ:1 ����ATָ��ʧ�ܣ� 0 ���� ATָ��ɹ�
�β�:u8 *at,���͵�ָ��
		 u16 timeout ��ʱ����
���ߣ�me
�汾��1.0 
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
				u2.len = 0;//��ֹ���ݽӵ�һ�룬û�н��꣬��ʱ�ͷ���0 ���¿�ʼ��
				return 1;
			}
		}
		u2.flag = 0;
		//��Ҫ��u2.buff ���� ok
		if(strstr((const char*)u2.buff,"OK") != NULL)
		{
			return 0;
		}			
	}
}


/*************************
�������ƣ�Wifi_Tcp_Init(void)
�������ܣ�TCP��ʼ��
����ֵ:��
�βΣ�void
���ߣ�me
�汾��1.0 
*************************/
void Wifi_Tcp_Init(void)
{
    u8 ret;
		printf("����Wifi_Tcp_Init\r\n");  // ����
    // ����ȫ�ڣ���ʾ��ʼ������
    Lcd_Clear(0, 0, 240, 320, BLACK);
    Lcd_Display_Str(10, 35, WHITE, BLACK, 16, (u8 *)"WiFi������...");

    Usart2_Init(115200);
    //�˳�����ģʽ
    USART2_SendString("+++");
    Delay_Ms(100);
    //����AT\r\n��������ʽ����
    USART2_SendString("AT\r\n");
    Delay_Ms(100);

    ret = Esp_SendData((u8 *)"AT+MQTTCLEAN=0\r\n", 2000);
    if(ret == 0)
    {
        printf("�Ͽ��������ɹ�\r\n");
        Lcd_Display_Str(10, 65, GREEN, BLACK, 16, (u8 *)"1.�Ͽ��ɷ�����  �ɹ�");
    }
    else
    {
        Lcd_Display_Str(10, 65, RED, BLACK, 16, (u8 *)"1.�Ͽ��ɷ�����  ʧ��");
    }

    //������WIFIģʽΪ�ͻ���ģʽ		AT+CWMODE=1\r\n
    ret = Esp_SendData((u8 *)"AT+CWMODE=1\r\n", 5000);
    if(ret == 0)
    {
        printf("����WIFIģʽΪ�ͻ���ģʽ�ɹ�\r\n");
        Lcd_Display_Str(10, 90, GREEN, BLACK, 16, (u8 *)"2.���ÿͻ���ģʽ  �ɹ�");
    }
    else
    {
        Lcd_Display_Str(10, 90, RED, BLACK, 16, (u8 *)"2.���ÿͻ���ģʽ  ʧ��");
    }

    //��������·�ɣ�����WIFI��AT+CWJAP=\"Class8\",\"12345678\"\r\n
    ret = Esp_SendData((u8 *)"AT+CWJAP=\"ahccqw\",\"88888888\"\r\n", 10000);
    if(ret == 0)
    {
        printf("����AP�ɹ�\r\n");
        Lcd_Display_Str(10, 115, GREEN, BLACK, 16, (u8 *)"3.����WiFi  �ɹ�");
    }
    else
    {
        Lcd_Display_Str(10, 115, RED, BLACK, 16, (u8 *)"3.����WiFi  ʧ��");
    }

    //�����ô���ģʽΪ͸��ģʽ	AT+CIPMODE=1
    ret = Esp_SendData((u8 *)"AT+CIPMODE=1\r\n", 10000);
    if(ret == 0)
    {
        printf("����Ϊ͸��ģʽ�ɹ�\r\n");
        Lcd_Display_Str(10, 140, GREEN, BLACK, 16, (u8 *)"4.����͸��ģʽ  �ɹ�");
    }
    else
    {
        Lcd_Display_Str(10, 140, RED, BLACK, 16, (u8 *)"4.����͸��ģʽ  ʧ��");
    }

    //������MQTT�û����Ӳ���
    ret = Esp_SendData((u8 *)"AT+MQTTUSERCFG=0,1,\"7ef12f698119426daa6289a1bdfd6042\",\"1ke8rhczmzyb8e9s\",\"ZBdO0RoYJ8\",0,0,\"\"\r\n", 10000);
    if(ret == 0)
    {
        printf("����MQTT�û��ɹ�\r\n");
        Lcd_Display_Str(10, 165, GREEN, BLACK, 16, (u8 *)"5.����MQTT�û�  �ɹ�");
    }
    else
    {
        Lcd_Display_Str(10, 165, RED, BLACK, 16, (u8 *)"5.����MQTT�û�  ʧ��");
    }

    //�����ӷ�����
    ret = Esp_SendData((u8 *)"AT+MQTTCONN=0,\"gz-3-mqtt.iot-api.com\",1883,1\r\n", 10000);
    if(ret == 0)
    {
        Lcd_Display_Str(10, 190, GREEN, BLACK, 16, (u8 *)"6.����MQTT������  �ɹ�");
    }
    else
    {
        Lcd_Display_Str(10, 190, RED, BLACK, 16, (u8 *)"6.����MQTT������  ʧ��");
    }

    //�޶����·�����
    ret = Esp_SendData((u8 *)"AT+MQTTSUB=0,\"attributes/push\",0\r\n", 1000);

    // ȫ�������ʾ
    Lcd_Display_Str(10, 260, WHITE, BLACK, 16, (u8 *)"��ʼ�����!");
    Delay_Ms(1500);  // ͣ��1.5�����û�������
}


/*************************
�������ƣ�Wifi_Control(void)
�������ܣ�Wifi���������� ���ܶ���Ϣ
����ֵ:��
�βΣ�void
���ߣ�me
�汾��1.0 
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
        u2_music_page4_handled = 0;   /* ��֡�������������ִ�����־ */
 
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
 
        /* -------- ɱ���� -------- */
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
 
        /* -------- ��� -------- */
        serve_motor_pos = strstr((char *)u2.buff, "\"servo_motor\":");
        if(serve_motor_pos != NULL)
        {
            sscanf(serve_motor_pos + 14, "%u", &serve_motor_value);
            Servo_motor_Control(serve_motor_value);
            cst.servo_state  = (serve_motor_value != 120) ? 1 : 0;
            cst.current_page = 3;
            cst.need_upload  = 1;
        }
 
        /* -------- ���� -------- */
        motor_pos = strstr((char *)u2.buff, "\"motor\":");
        if(motor_pos != NULL)
        {
            sscanf(motor_pos + 8, "%u", &motor_value);
            cst.motor_value      = motor_value;
            cst.motor_state      = (motor_value != 0) ? 1 : 0;
            cst.need_upload      = 1;
            TIM_SetCompare3(TIM3, motor_value);
            cst.last_motor_state = cst.motor_state - 1;   /* ǿ�ƴ���ˢ�� */
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
         * -------- ����ָ�� --------
         * ����
         *   - ��ǰ��������ҳ�棨page4������ѭ��ֱ�Ӵ������л�ҳ��
         *   - ��ǰ������ҳ�棨page4����Audio_MusicPlay() ����������
         *     ָ������ SysTick �жϴ�����u2_music_page4_handled=1����
         *     ��ѭ�������������ظ�ִ��
         */
        if(cst.current_page != 4)
        {
						
            /* ��������ҳ�棬��ѭ���������� */
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
				// �򵥷����������ǰ��������ҳ�棬�϶�û��������ֱ�ӽ���
				if (cst.current_page != 4)
				{
						u2_music_page4_handled = 0; 
				}
				// ���׷��������������ҳ�棬������״̬�Ѿ����������� STOP����Ҳ���Խ���
				else if (status_dev.PlayState == PLAY_STOP)
				{
						 u2_music_page4_handled = 0;
				}
        /* ������ҳ��ʱ���ж��Ѿ��������˴����ظ�ִ�� */
    }
}


/*************************
�������ƣ�Wifi_Send_Message(void)
�������ܣ�Wifi��ʱ���ͳ�����(��ʪ��)��������
����ֵ:��
�βΣ�void
���ߣ�me
�汾��1.0 
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
�������ƣ�Status_Upload(void)
�������ܣ����״̬�仯���ϴ���ƽ̨����ѭ���е���
����ֵ����
�βΣ���
���ߣ�me
�汾��1.0
*************************/
void Status_Upload(void)
{
    static u32 last_upload_time = 0;
    u32 now = Get_Ms();

    // ����Ƶ�ʣ�3�룩
    if((now - last_upload_time) < 3000) return;
    if(cst.need_upload == 0 && cst.first_upload == 1) return;

    last_upload_time = now;
    cst.need_upload = 0;

    char pub_buff[256] = {0};
    char send_buff[100] = {0};
    char temp[64];
    int first = 1;

    strcat(pub_buff, "{");

    /******************  �״Σ�ȫ���ϴ� ******************/
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

        //  ͬ�����ϴ�״̬��
        cst.last_led_upload = cst.led_state;
        cst.last_motor_upload = cst.motor_value;
        cst.last_servo_upload = cst.servo_state;
        cst.last_rgb_upload = cst.rgb_value;
        cst.last_rgb_upload_state = cst.rgb_state;
        cst.last_germicidal_upload = cst.Germicidal_state;

        cst.first_upload = 1;  // ������ϴ�
    }
    else
    {
        /******************  �����ϴ� ******************/

        /***** LED *****/
        if(cst.led_state != cst.last_led_upload)
        {
            cst.last_led_upload = cst.led_state;

            sprintf(temp, "\"Led\":%d", cst.led_state);
            if(!first) strcat(pub_buff, ",");
            strcat(pub_buff, temp);
            first = 0;
        }

        /***** MOTOR����ֵ + fan��*****/
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

        /***** RGB����ֵ + rgb_ct��*****/
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

        /***** ������ *****/
        if(cst.Germicidal_state != cst.last_germicidal_upload)
        {
            cst.last_germicidal_upload = cst.Germicidal_state;

            sprintf(temp, "\"Germicidal_lamp\":%d", cst.Germicidal_state);
            if(!first) strcat(pub_buff, ",");
            strcat(pub_buff, temp);
            first = 0;
        }

        strcat(pub_buff, "}");

        // ? û�仯����
        if(strlen(pub_buff) <= 2)
            return;
    }

    /****************** ? MQTT���� ******************/
    sprintf(send_buff,
        "AT+MQTTPUBRAW=0,\"attributes\",%d,0,0\r\n",
        strlen(pub_buff)
    );

    Esp_SendData(send_buff, 500);
    Esp_SendData(pub_buff, 500);
}












