#include "rs485_modbus_master.h"

Slave modbus_slave;
Modebus modbus_master;
/*************************
函数名称：U4_Modbus_Init(u32 baud)
函数功能：Usart4_Modbus初始化函数
返回值:无
形参：u32 baud
作者：me
版本：1.0 
		U4 RX - PC11		U4	TX - PC10		APB2 42Mhz
*************************/
void U4_Modbus_Init(u32 baud)
{
    // 1. 打开时钟
    // STM32F4系列 USART4 在 APB1
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    // 2. 初始化 IO 口
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;       // 复用功能
    GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;   // 高速
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;     // 推挽输出 (默认)
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;       // 上拉 (建议开启，防止悬空干扰)
    GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	  //    配置 PE1 RS485状态控制引脚
    GPIO_InitTypeDef GPIO_RS485_Ctrl;
    GPIO_RS485_Ctrl.GPIO_Pin = GPIO_Pin_1;          // 引脚 PE1
    GPIO_RS485_Ctrl.GPIO_Mode = GPIO_Mode_OUT;      // 输出模式
    GPIO_RS485_Ctrl.GPIO_Speed = GPIO_High_Speed;   // 高速，确保快速切换
    GPIO_RS485_Ctrl.GPIO_OType = GPIO_OType_PP;     // 推挽输出
    GPIO_RS485_Ctrl.GPIO_PuPd = GPIO_PuPd_NOPULL;   // 无上拉下拉
    GPIO_Init(GPIOE, &GPIO_RS485_Ctrl);

    // 3. 引脚复用映射
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
    
    // 4. 初始化 USART 控制器
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = baud;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;    // 无校验
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // Modbus 必须无流控
    USART_Init(UART4, &USART_InitStruct);
    
    // 5. 接收中断使能
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
    
    // 6. NVIC 配置 (修正核心错误)
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = UART4_IRQn;       
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 7;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    
    // 7. 使能串口
    USART_Cmd(UART4, ENABLE);
}

//串口中断用来接收数据
void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4,USART_IT_RXNE)==SET)
	{
		USART_ClearITPendingBit(UART4,USART_FLAG_RXNE);
		
		//接收数据		进来一次 长度+1
		modbus_master.rxbuff[modbus_master.rxcount++] = USART_ReceiveData(UART4);
		
		//只要开始接收数据就 启动tim7定时器 及 重置计数器 ， 相当于给TIM7喂狗的作用
		if(modbus_master.rxcount == 1)
		{
			TIM_Cmd(TIM7, ENABLE);
		}
		
		//只要收到数据就一直重置计速器
		TIM_SetCounter(TIM7, 0);
	}
}


//RS485底层驱动
void RS485_SendData(u8 *ptx,u8 lenth)
{
	u32 i;
	//切换发送模式 切位RS485SEND 也就是状态位置1
	RS485_SEND;
	
	//逐字节发送数据
		for(i=0;i<lenth;i++)
	{
		USART_SendData(UART4,ptx[i]);
	}
	while(USART_GetFlagStatus(UART4,USART_FLAG_TC) == RESET);//RESET为发送完成
	
	//切回接收模式
	RS485_RESE;
}

//与从机通信发送对应的功能码 来 获取到对应的信息
void ModbusMaster_Transmit(void)
{
	u8 i=0;
	u16 crc=0;
	//构建数据帧
	modbus_master.rxbuff[i++] = MODBUS_ID;//设备地址
	modbus_master.rxbuff[i++] = 0x03;//功能码
	modbus_master.rxbuff[i++] = 0x00;//开始地址 高8位
	modbus_master.rxbuff[i++] = 0x01;//开始地址 低8位
	modbus_master.rxbuff[i++] = 0x00;//寄存器数量 高8位
	modbus_master.rxbuff[i++] = 0x05;//寄存器数量 低8位
	
	//计算前面组装好的六个字节的数据计算crc校验值	
	crc = crc16_modbus(modbus_master.rxbuff, i);
	modbus_master.rxbuff[i++] = (crc >> 8) & 0xff;//低位先发的 所以需要低位移到最右
	modbus_master.rxbuff[i++] = crc & 0xff;
	modbus_master.txcount = i;
	
	//发送数据及校验值 校验值先发低八位再发高八位
	RS485_SendData(modbus_master.rxbuff, modbus_master.txcount);
}

//数据接收及处理
void ModbusMaster_Receive(void)
{
	/*
		从机收到信息后的返回的响应帧格式如下
		rxbuff[0] - 地址
		rxbuff[1] - 功能码
		rxbuff[2] - 字节数
		rxbuff[3] - 数据1 高8位
		rxbuff[4] - 数据1 低八位
		.
		.
	*/
	u8 i=0;
	u16 crc_curent=0;
	u16 crc_recevice=0;
	
	//检测 rxover结束的标志位 为0则退出
	if(modbus_master.rxover == 0) return;
	printf("rx_count= %d \r\n",modbus_master.rxcount);
	
	//调试信息打印
	for(i=0;i<modbus_master.rxcount;i++)
	{
		printf("rxbuff[%d] = %x \r\n",i,modbus_master.rxbuff[i]);
	}
	
	//对当前数据进行crc校验，并保存对应的校验码方便后续比较，这里-2是为了取消掉后两位的校验
	crc_curent = crc16_modbus(modbus_master.rxbuff,modbus_master.rxcount-2);
	
	//获取到发来的校验码与生成的校验码进行比较
	crc_recevice = (modbus_master.rxbuff[modbus_master.rxcount-2] << 8)  |	(modbus_master.rxbuff[modbus_master.rxcount-1]);
	
	if(crc_curent != crc_recevice)	goto MODBUS_ERROR;
	printf("crc校验成功！");
	//解析数据
	switch(modbus_master.rxbuff[1])
	{
		case 0x03:
			if(MODBUS_ID == modbus_master.rxbuff[0])//判断ID是否相同
			{
				
				modbus_slave.buff = modbus_master.rxbuff[3] << 8 | modbus_master.rxbuff[4];
					
			}
	}
	
	//清理状态：错误状态 以及 该函数运行到此步骤的时候 执行以下代码 清空接收数据及接收状态重新接收
MODBUS_ERROR:
	modbus_master.rxcount = 0;
	modbus_master.rxover = 0;
	modbus_master.txcount = 0;
}






