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
	
	Usart2_Init(115200);
	//退出发送模式
	USART2_SendString("+++");
	Delay_Ms(100);
	//发送AT\r\n，跳过格式报错
	USART2_SendString("AT\r\n");
	Delay_Ms(100);
	//①设置WIFI模式为客户端模式		AT+CWMODE=1\r\n
	ret = Esp_SendData((u8 *)"AT+CWMODE=1\r\n",5000);
	if(ret == 0)	printf("设置WIFI模式为客户端模式成功\r\n");
	//②连接软路由（连接WIFI）AT+CWJAP=\"Class8\",\"12345678\"\r\n
	ret = Esp_SendData((u8 *)"AT+CWJAP=\"Class8\",\"12345678\"\r\n",10000);
	if(ret == 0)	printf("连接AP成功\r\n");
	//③设置传输模式为透传模式	AT+CIPMODE=1
	ret = Esp_SendData((u8 *)"AT+CIPMODE=1\r\n",10000);
	if(ret == 0)	printf("设置为透传模式成功\r\n");
	//④连接服务器	AT+CIPSTART=\"TCP\",\"192.168.5.26\",5789\r\n
	ret = Esp_SendData((u8 *)"AT+CIPSTART=\"TCP\",\"192.168.5.26\",5789\r\n",10000);
	if(ret == 0)	printf("服务器连接成功\r\n");
	//⑤进入透传，开始传输数据到服务器  发送数据模试AT+CIPSEND\r\n
	Esp_SendData((u8 *)"AT+CIPSEND\r\n",10000);
	if(ret == 0)	printf("进入发送数据模式成功\r\n");


}















