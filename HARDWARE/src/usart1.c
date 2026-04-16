#include "usart1.h"

U1 u1;//定义结构体变量才能使用
/*************************
函数名称：Usart1_Init(void)
函数功能：Usart1初始化函数
返回值:无
形参：u32 baud
作者：me
版本：1.0 
		U1 TX - PA9		U1	RX - PA10		APB2 84Mhz
*************************/
void Usart1_Init(u32 baud)
{
	//打开串口/IO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//初始化IO口 复用模式	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);//PA9服用到USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);//PA10服用到USART1
	
	//初始化USART1控制器
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = baud;		//波特率
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//启用发送及接收功能
	USART_InitStruct.USART_Parity = USART_Parity_No;		//无校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;		//一位停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;		//8位数据位
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// 无硬件流控制	
	USART_Init(USART1, &USART_InitStruct);
	
	//中断使能
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//接收中断，接收数据断，接受到一个字节触发
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);//空闲中断，接收结束后经过1个帧还没有数据的时候为1,
	
	//NVIC
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;//要配置的中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 7;//抢占优先级（值越小优先级越高）
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//次级优先级（值越小优先级越高）
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能，失能中断
	NVIC_Init( &NVIC_InitStruct);
	
	//U1 使能
	USART_Cmd(USART1,ENABLE);
}

/*************************
函数名称：USART1_IRQHandler(void)
函数功能：USART1接收中断服务函数
返回值：无
形参：无
作者：me
版本：1.0 
函数说明:这里结构体由于是全局的变量，则初始值为0	
*************************/
void USART1_IRQHandler(void)
{
//	u8 data = 0;
//	static u32 addr = 0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE))//如果接收到了数据	0中断未触发 1触发
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);//清除接收中断标志位
		u1.buff[u1.len++] = USART_ReceiveData(USART1);
		
//		Re_flag = 1;
//		data = USART_ReceiveData(USART1);
//		w25_Wr_Ct(addr++,&data,1);
		
	}
	if(USART_GetITStatus(USART1, USART_IT_IDLE))
	{
		USART1->SR;//清标志位
		USART1->DR;//清标志位
//		if(addr >= 0x001D3374)
//		{	
//			Re_flag = 0;
//			Re_End_flag = 0;
//			addr = 0;
//		}
		
		USART2_SendString((char *)u1.buff);
		
//		u1.buff[u1.len] = '\0';
//		printf("%s", u1.buff);  // 打印接收到的数据
//		u1.len = 0;
	}
}


/*************************
函数名称：Usart1_SendByte(u8 data)
函数功能：Usart1字节发送函数
返回值:无
形参：u8 data
作者：me
版本：1.0 
*************************/
void Usart1_SendByte(u8 data)
{
	//等待上一个字节发送完成 完成的时候为0 则需非
	while(!(USART_GetFlagStatus(USART1, USART_FLAG_TXE)));//TXE发送寄存器为空
	USART_SendData(USART1,data);
	//等待这个字节发送完成
	while(!(USART_GetFlagStatus(USART1, USART_FLAG_TC)));//TC发送完成
}

/*************************
函数名称：Usart1_SendString(u32 data)
函数功能：Usart1字符串发送函数
返回值:无
形参：u32 data
作者：me
版本：1.0 
*************************/
void Usart1_SendString(char *str)
{	
	    while(*str != '\0')
    {
        Usart1_SendByte(*str++);
    }
		Usart1_SendByte('\0');//最后一个字节补发\0
}

/*************************
函数名称：int fputc(FILE)
函数功能：printf重定向
返回值：无
形参：无
作者：me
版本：1.0 
函数说明:printf("a:%d",a)
//不需要声明调用
*************************/
int fputc(int c, FILE * stream)
{
	Usart1_SendByte(c);
	return c;
}

/*************************
函数名称：Usart1_RecByte(void)
函数功能：Usart1接收字符
返回值:u16
形参：void
作者：me
版本：1.0 
*************************/
u8 Usart1_RecByte()
{
	u8 data;
	// (1) 表示有数据来了，为 (0)表示没数据	--	应该等待它从 0 变成 1（即：当它是 0 时一直等）
	while(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE)));
	data = USART_ReceiveData(USART1);
	return data;
}

/*************************
函数名称：Usart1_RecStr(u8 *str)
函数功能：Usart1接收字符串数据
返回值：void
形参：u8 *str 传递接收到的数据 
作者：me
版本：1.0 
函数说明:接收字符串
*************************/
void Usart1_RecStr(u8 *str)
{
	u8 data;
	while(1)
	{
		data = Usart1_RecByte();//一个字节一个字节去接收
		if(data == '\r' || data == '\n')
		{
			*(str+1) = 0;
			*str = '\0';
			return;
		}
		*str = data;
		str++;		
	}	
}

