#include "w25q64.h"





void W25q64_Init(void)
{
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT; 
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	Spi1_Init();
	
	W25Q64_CS_H;
	
}




/*************************
函数名称：w25_Change_By(u8 data)
函数功能：w25读ID
返回值：u8
形参：u8 data
作者：me
版本：1.0 
*************************/
void w25_Change_By(void)
{
		u16 ReId = 0;
		W25Q64_CS_L;
		//发送0x90读ID指令
		Spi1_Change_By(0x90);
		//发送24位0 地址分三次发送
		Spi1_Change_By(0);
		Spi1_Change_By(0);
		Spi1_Change_By(0);
		//接收厂家ID
		ReId = Spi1_Change_By(0xff);
		//接收设备ID
		ReId = ReId << 8 | Spi1_Change_By(0xff);
		//结束拉高CS
		W25Q64_CS_H;
		//打印 ID %x
		printf("Factory_ID:%x",ReId);
}

/*************************
函数名称：w25_Wr_Enable(void)
函数功能：w25写使能函数
返回值：无
形参：无
作者：me
版本：1.0 
*************************/
void w25_Wr_Enable(void)
{
	//拉低,接收	
	W25Q64_CS_L;
	//发送0x06 写使能指令
	Spi1_Change_By(0x06);
	//拉高停止
	W25Q64_CS_H;
}

/*************************
函数名称：w25_Re_Free(void)
函数功能：w25读空闲函数		返回为0是空闲
返回值:u8
形参：无
作者：me
版本：1.0 
*************************/
u8 w25_Re_Free(void)
{
	u8 temp;
	//拉低，接收
	W25Q64_CS_L;
	//发送读函数指令
	Spi1_Change_By(0x05);
	//接收数据
	temp = Spi1_Change_By(0xff);
	//拉高停止
	W25Q64_CS_H;
	//判断空闲
	if(temp & (0x01))
	{
		return 1;
	}
	return 0;
}

/*************************
函数名称：w25_Erase(void)
函数功能：w25擦除函数
返回值：无												4096								65536byte
形参：u32 addr,cmd  : 擦除指令 (0x20=扇区, 0x52=半块,0xD8=块, 0xC7=整片)
作者：me
版本：1.0 
*************************/
void w25_Sector_Erase(u32 addr,u8 cmd)
{		
	//写	WEL位 0 是 芯片拒绝执行任何写入、擦除或写状态寄存器的指令。且与写使能是绑定的所有一开始就要写使能
	w25_Wr_Enable();
	
	while(w25_Re_Free());//等待空闲

	W25Q64_CS_L;
	//发送擦除指令
	Spi1_Change_By(cmd);
	if(cmd != 0xC7)
	{
	Spi1_Change_By((addr >> 16) & 0xff);
	Spi1_Change_By((addr >> 8) & 0xff);
	Spi1_Change_By(addr & 0xff);
	}
	//拉高
	W25Q64_CS_H;
	while(w25_Re_Free());//等待扇区擦除完成
}

/*************************
函数名称：w25_Wr_Pg(u8 *data,u32 addr,u32 len)
函数功能：w25页写函数
返回值：无
形参：u8 *data,u32 addr,u32 len
作者：me
版本：1.0 
*************************/
void w25_Wr_Pg(u32 addr,u8 *data,u32 len)
{
	w25_Wr_Enable();
	while(w25_Re_Free());//等待空闲
	W25Q64_CS_L;
	//发送擦除指令
	Spi1_Change_By(0x02);
	//发送写入地址
	Spi1_Change_By((addr >> 16) & 0xff);
	Spi1_Change_By((addr >> 8) & 0xff);
	Spi1_Change_By(addr & 0xff);
	while(len -- )
	{
		Spi1_Change_By(*data);
		data++;
		
	}
	//拉高
	W25Q64_CS_H;
	while(w25_Re_Free());//等待空闲
}

/*************************
函数名称：w25_Re_Pg(u8 *data,u32 addr,u32 len)
函数功能：w25页读函数
返回值：无
形参：无
作者：me
版本：1.0 
*************************/
void w25_Re_Pg(u32 addr,u8 *data,u32 len)
{
	W25Q64_CS_L;
	//发送擦除指令
	Spi1_Change_By(0x03);
	//发送写入地址
	Spi1_Change_By((addr >> 16) & 0xff);
	Spi1_Change_By((addr >> 8) & 0xff);
	Spi1_Change_By(addr & 0xff);
	while(len --)
	{
		*data = Spi1_Change_By(0xff);
		data++;//地址偏移
	}
	//拉高
	W25Q64_CS_H;
}

/*************************
函数名称：w25_Wr_Ct(u8 *data,u32 addr,u32 len)
函数功能：w25跨页写函数
返回值：无
形参：u8 *data,u32 addr,u32 len
作者：me
版本：1.0 
*************************/
void w25_Wr_Ct(u32 addr,u8 *data,u32 len)
{
	u32 Re_Wr;
	u32 Max_Space;

	while(len)
	{
		Max_Space = 256 -  (addr % 256);
		if(Max_Space > len)
		{
			Re_Wr = len;
		}
		else
		{
			Re_Wr = Max_Space;
		}
		
		
		w25_Wr_Enable();
		while(w25_Re_Free());//等待空闲
		W25Q64_CS_L;
		//发送擦除指令
		Spi1_Change_By(0x02);
		//发送写入地址
		Spi1_Change_By((addr >> 16) & 0xff);
		Spi1_Change_By((addr >> 8) & 0xff);
		Spi1_Change_By(addr & 0xff);
	
		for(u32 i=0;i<Re_Wr;i++)
		{
		Spi1_Change_By(data[i]);

		}
		
		//拉高
		W25Q64_CS_H;
		while(w25_Re_Free());//等待空闲
			
		addr += Re_Wr;
		data += Re_Wr;
		len  -= Re_Wr;
		
	}
}


u8 Re_flag = 0;
u8 Re_End_flag = 1;
/*************************
函数名称：Font_Updata(void)
函数功能：更新字库
返回值：无
形参：无
作者：me
版本：1.0 
*************************/
void Font_Updata(void)
{ 
	printf("正在擦除,请稍后......\r\n");
	for(u8 i=0;i<30;i++)
	{
		w25_Sector_Erase(65536*i,0xD8);
	}
	printf("擦除完成，请发送文件\r\n");
	
	while(!Re_flag);// Re_flag = 1;表示正在接收中，此时卡住，等待接完
	printf("正在接收中，请稍后...\r\n");
	
	while(Re_End_flag);//	Re_End_flag = 0;表示未接收完成，此时卡住
	printf("接收完成！！！\r\n");
		
}
	
























