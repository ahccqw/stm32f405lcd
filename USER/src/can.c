#include "can.h"

/*************************
函数名称：Can_Config(void)
函数功能：Can总线配置
返回值:无
形参：无
作者：me
版本：1.0 
PA11 -- CAN1RX		PA12 -- CAN1TX
*************************/
void Can_Config(u8 BS1,u8 BS2,u8 Mode,u16 psc)
{
	//打开时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//打开GPIOA
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//打开CAN1
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	CAN_FilterInitTypeDef CAN_FilterInitStruct = {0};
	
	//初始化IO口 复用推挽模式	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1);//PA11复用到CAN1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1);//PA12复用到CAN1
	
	//初始化配置CAN
	CAN_InitTypeDef CAN_InitStruct = {0};
	CAN_InitStruct.CAN_ABOM = ENABLE;//自动关闭总线
	CAN_InitStruct.CAN_AWUM = DISABLE;//自动唤醒
	CAN_InitStruct.CAN_BS1 = BS1;
	CAN_InitStruct.CAN_BS2 = BS2;
	CAN_InitStruct.CAN_Mode = Mode;//模式
	CAN_InitStruct.CAN_NART = ENABLE;//无论发送的结果如何 消息均只发送一次
	CAN_InitStruct.CAN_Prescaler = psc -1;//预分频
	CAN_InitStruct.CAN_RFLM = DISABLE;//接收FIFO装满后，下一条传入信息将覆盖前一条信息
	CAN_InitStruct.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStruct.CAN_TTCM = DISABLE;//时间触发通信模式
	CAN_InitStruct.CAN_TXFP = DISABLE;
	u8 temp = CAN_Init(CAN1, &CAN_InitStruct);
	
	//初始化配置 CAN的筛选器
	CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;//激活或禁用这个筛选器
	CAN_FilterInitStruct.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//筛选后，被存入哪个接收FIFO（先进先出队列）0的优先级高，用来存安全气囊 ，1为日常的信息
	CAN_FilterInitStruct.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStruct.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStruct.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask;//设置筛选器的工作模式,使用“ID”和“掩码”来定义筛选规则
	CAN_FilterInitStruct.CAN_FilterNumber = 0;//要配置的筛选器编号（也称为筛选器组）
	CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_32bit;
	
	CAN_FilterInit(&CAN_FilterInitStruct);
}

/*************************
函数名称：Can_Send_Message(u8 *sendmesg)
函数功能：Can发送信息
返回值:无
形参：无
作者：me
版本：1.0 
*************************/
u8 Can_Send_Message(u8 *sendmesg,u8 len)
{
	u16 i;
	CanTxMsg TxMessage = {0};

	TxMessage.DLC = len;//数据长度
	TxMessage.IDE = CAN_Id_Standard;// 0为标准ID ， 1为拓展ID
	TxMessage.RTR = CAN_RTR_Data;// 0数据帧 ， 摇控帧
	TxMessage.StdId = 0x14;

	for(i=0;i<len;i++)
	{
		TxMessage.Data[i]=sendmesg[i];
	}
	u8 Mailbox = CAN_Transmit(CAN1, &TxMessage);
	//判断发送失败和超时的情况下退出
	while(CAN_TransmitStatus(CAN1,Mailbox)==CAN_TxStatus_Failed && i<0xfff)
	{
		i++;
	}
	if(i>=0xfff)
	{
		return 1;
	}
	return 0;
}

/*************************
函数名称：Can_Res_Message(u8 *resmesg)
函数功能：Can接收信息
返回值:无
形参：无
作者：me
版本：1.0 
*************************/
u8 Can_Res_Message(u8 *resmesg)
{
	u8 i;
	CanRxMsg RxMessage = {0};
	while(CAN_MessagePending(CAN1,CAN_FIFO0)==0)
	{
		return 0;
	}
	//第二个参数 FIFONumber 用于指定从哪个接收 FIFO（First-In, First-Out 缓冲区）中读取 CAN 报文。	CAN_FIFO0 -- CAN_FIFO1
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	for(i=0;i<RxMessage.DLC;i++)
	{
		resmesg[i] = RxMessage.Data[i];
	}
	
	return RxMessage.DLC;
}
