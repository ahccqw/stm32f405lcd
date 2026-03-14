#include "lcd.h"
#include "pic.h"
#include "ikun.h"


/*************************
函数名称：lcd_Port_Init(void)
函数功能：lcd端口初始化
返回值：无
形参：无
作者：me
版本：1.0 
	
	LCD_SCK		---		PB3		---		通用推挽输出
	LCD_CS		---		PB4		---		通用推挽输出
	LCD_SDI		---		PB5		---		通用推挽输出
	LCD_DC		---		PD3		---		通用推挽输出
	LCD_BLK		---		PD4		---		通用推挽输出		-- 背光使能
*************************/
void lcd_Port_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOB, ENABLE);//打开IO时钟
	
	//结构体变量的定义
	GPIO_InitTypeDef GPIO_InitStruct={0};	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上拉下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	LCD_CS_H;
}

/*************************
函数名称：Lcd_TransferData(u8 data)
函数功能：Lcd 交换数据
返回值：无
形参：无
作者：me
版本：1.0 
*************************/
void Lcd_TransferData(u8 data)
{	
	for(u8 i=0;i<8;i++)
	{
		LCD_SCK_L;
		if(data & (0x80 >> i))
		{
			LCD_SDI_H;
		}
		else
		{
			LCD_SDI_L;
		}
		LCD_SCK_H;
	}
}

/*************************
函数名称：Lcd_SendCmd(u8 data)
函数功能：Lcd 发送命令
返回值：无
形参：无
作者：me
版本：1.0 
*************************/
void Lcd_SendCmd(u8 data)
{
	LCD_CMD;
	LCD_CS_L;
	Lcd_TransferData(data);
	LCD_CS_H;
}

/*************************
函数名称：Lcd_SendData(u8 data)
函数功能：Lcd 发送8位数据/参数
返回值：无
形参：无
作者：me
版本：1.0 
*************************/
void Lcd_Send_Data(u8 data)
{
	LCD_DATA;
	LCD_CS_L;
	Lcd_TransferData(data);
	LCD_CS_H;
}

/*************************
函数名称：Lcd_SendData(u16 data)
函数功能：Lcd 发送16位数据/参数
返回值：无
形参：无
作者：me
版本：1.0 
*************************/
void Lcd_SendData_16B(u16 data)
{
	LCD_DATA;
	LCD_CS_L;
	Lcd_TransferData(data >> 8);
	Lcd_TransferData(data);
	LCD_CS_H;
}

/*************************
函数名称：Lcd_SetPosition(u16 xs,u16 ys,u16 xe,u16 ye)
函数功能：Lcd 显示位置
返回值：无
形参：无
作者：me
版本：1.0 
*************************/
void Lcd_SetPosition(u16 xs,u16 ys,u16 xe,u16 ye)
{
	Lcd_SendCmd(0x2a);
	Lcd_SendData_16B(xs);
	Lcd_SendData_16B(xe-1);

	Lcd_SendCmd(0x2b);
	Lcd_SendData_16B(ys);
	Lcd_SendData_16B(ye-1);
	
	Lcd_SendCmd(0x2c);
}

/*************************
 * 函 数 名：LCD_Init
 * 函数功能：LCD初始化
 * 入口参数：无
 * 出口参数：无
 * 备    注：包含GPIO初始化、SPI初始化和LCD参数设置，以及初始化显示
*************************/
void LCD_Init(void)
{
	//GPIO管脚初始化
	lcd_Port_Init();
	
	LCD_BLK_ON;
	Delay_Ms(100);
	
	//************* Start Initial Sequence **********//
	Lcd_SendCmd(0x11); //Sleep out 
	Delay_Ms(120);     //Delay 120ms 
	
	//************* Start Initial Sequence **********// 
	Lcd_SendCmd(0x36);	//设置扫描方向。0x00 通常代表从上到下
	Lcd_Send_Data(0x00);

	Lcd_SendCmd(0x3A); //设置像素格式。0x05 代表 16-bit/pixel (RGB565)，这是最常用的格式。
	Lcd_Send_Data(0x05);

	Lcd_SendCmd(0xB2);
	Lcd_Send_Data(0x0C);
	Lcd_Send_Data(0x0C);
	Lcd_Send_Data(0x00);
	Lcd_Send_Data(0x33);
	Lcd_Send_Data(0x33); 

	Lcd_SendCmd(0xB7); 
	Lcd_Send_Data(0x35);  

	Lcd_SendCmd(0xBB);
	Lcd_Send_Data(0x19);

	Lcd_SendCmd(0xC0);
	Lcd_Send_Data(0x2C);

	Lcd_SendCmd(0xC2);
	Lcd_Send_Data(0x01);

	Lcd_SendCmd(0xC3);
	Lcd_Send_Data(0x12);   

	Lcd_SendCmd(0xC4);
	Lcd_Send_Data(0x20);  

	Lcd_SendCmd(0xC6); 
	Lcd_Send_Data(0x0F);    

	Lcd_SendCmd(0xD0); 
	Lcd_Send_Data(0xA4);
	Lcd_Send_Data(0xA1);

	Lcd_SendCmd(0xE0);
	Lcd_Send_Data(0xD0);
	Lcd_Send_Data(0x04);
	Lcd_Send_Data(0x0D);
	Lcd_Send_Data(0x11);
	Lcd_Send_Data(0x13);
	Lcd_Send_Data(0x2B);
	Lcd_Send_Data(0x3F);
	Lcd_Send_Data(0x54);
	Lcd_Send_Data(0x4C);
	Lcd_Send_Data(0x18);
	Lcd_Send_Data(0x0D);
	Lcd_Send_Data(0x0B);
	Lcd_Send_Data(0x1F);
	Lcd_Send_Data(0x23);

	Lcd_SendCmd(0xE1);
	Lcd_Send_Data(0xD0);
	Lcd_Send_Data(0x04);
	Lcd_Send_Data(0x0C);
	Lcd_Send_Data(0x11);
	Lcd_Send_Data(0x13);
	Lcd_Send_Data(0x2C);
	Lcd_Send_Data(0x3F);
	Lcd_Send_Data(0x44);
	Lcd_Send_Data(0x51);
	Lcd_Send_Data(0x2F);
	Lcd_Send_Data(0x1F);
	Lcd_Send_Data(0x1F);
	Lcd_Send_Data(0x20);
	Lcd_Send_Data(0x23);

	Lcd_SendCmd(0x21); 
	Lcd_SendCmd(0x11); 
	Lcd_SendCmd(0x29); 
	
	Lcd_Clear(0,0,240,320,WHITE);
}

/*************************
 * 函 数 名：Lcd_Clear(u16 color,u16 xs,u16 ys,u16 xe,u16 ye);
 * 函数功能：LCD清屏函数
 * 参数：u16 color,u16 xs,u16 ys,u16 xe,u16 ye
 * 返回值：无
 * 备    注：LCD清屏函数，制定位置清屏
*************************/
void Lcd_Clear(u16 xs,u16 ys,u16 xe,u16 ye,u16 color)
{
	Lcd_SetPosition(xs,ys,xe,ye);
	for(u32 i=0;i<(xe-xs)*(ye-ys);i++)
	{
		Lcd_SendData_16B(color);
	}
}

/*************************
 * 函 数 名：Lcd_Display_Photo(u16 x,u16 y,u8 *pic);
 * 函数功能：LCD显示任意大小图片
 * 参数:u16 x			X轴起始坐标
			  u16 y   	Y轴起始坐标
				u8 *pic		指向图片数据
 * 返回值：无
 * 备    注：
*************************/
void Lcd_Display_Photo(u16 x,u16 y,u8 *pic)
{
	u16 w = *(pic+2) << 8 | *(pic+3);
	u16 h = *(pic+4) << 8 | *(pic+5);
	
	Lcd_SetPosition(x,y,x+w,y+h);
	
	for(u32 i=0;i<w*h;i++)
	{
		Lcd_SendData_16B((pic[8 + i*2]<<8) | (pic[8 + i*2+1]));
	}
}

/*************************
函数名称：Lcd_Set_One_Position(u16 XS,u16 YS,u16 XE,u16 YE)
函数功能：Lcd 单个显示位置
返回值：无
形参：无
作者：me
版本：1.0 
*************************/
void Lcd_Set_One_Position(u16 x,u16 y,u16 color)
{
	Lcd_SetPosition(x,y,x+1,y+1);
	Lcd_SendData_16B(color);
}

/*************************
 * 函 数 名：Lcd_Display_Char(u16 x,u16 y,u16 font_color,u16 bg_color,u8 *eng)
 * 函数功能：Lcd显示字符
 * 参数:u16 x			X轴起始坐标
			  u16 y   	Y轴起始坐标
				u16 font_color 字体颜色
				u16 bg_color 背景颜色
u8 size		字体大小  16:8 * 16 ,24:12 × 24 ,32:16 × 32 	行为：8,16,16
				u8 *pic		指向图片数据
 * 返回值：无
 * 备    注：
*************************/
void Lcd_Display_Char(u16 x,u16 y,u16 font_color,u16 bg_color,u8 size,u8 eng)
{
	u8 i,j,k;
	u8 buff[70]={0};
	u32 addr=0;
	u8 row_size = 0;

	row_size = size / 2 / 8;		//计算每一行有多少字节数据
	
	if(size / 2 % 8 != 0)
	{
		row_size += 1;
	}
	
	switch(size)
	{
		case 16:addr += 0x001CF362;break;
		case 24:addr += 0x001CFB68;break;
		case 32:addr += 0x001D136E;break;
	}
	
	addr += eng * row_size * size;
	w25_Re_Pg(addr,buff,row_size * size);
	
		for(i=0;i<size;i++)//先循环行数 长
		{
			for(k=0;k<row_size;k++)//决定这一行中哪一字节数据
			{
					for(j=0;j<8;j++)//再循环列 宽
					{
						if(buff[i*row_size + k] & 0x80 >> j) // 第i行 * 每行的字节
						{
							Lcd_Set_One_Position(x+j+8*k,y+i,font_color);// 8 * k 为这一行的第几个字节
						}
						else
						{
							Lcd_Set_One_Position(x+j+8*k,y+i,bg_color);
						}
					}
			}
			
		}
}

/*************************
 * 函 数 名：Lcd_Display_Chinese_Char(u16 x,u16 y,u16 font_color,u16 bg_color,u8 *eng)
 * 函数功能：Lcd显示字符
 * 参数:u16 x			X轴起始坐标
			  u16 y   	Y轴起始坐标
				u16 font_color 字体颜色
				u16 bg_color 背景颜色
u8 size		字体大小  16:16 * 16 ,24:24 × 24 ,32:32 × 32
				u8 *pic		指向图片数据
 * 返回值：无
 * 备    注：
*************************/
void Lcd_Display_Chinese_Char(u16 x,u16 y,u16 font_color,u16 bg_color,u8 size,u8 *chn)
{
	u8 i,j,k;
	u8 buff[130]={0};
	u32 addr=0;
	u8 row_size = 0;

	row_size = size  / 8;		//计算每一行有多少字节数据
	
	if(size % 8 != 0)
	{
		row_size += 1;
	}
	
	switch(size)
	{
		case 16:addr += 0x00000000;break;
		case 24:addr += 0x0003FE46;break;
		case 32:addr += 0x000CFA5C;break;
	}
	
	addr += ((chn[0] - 0xA1)*94 + (chn[1] - 0xA1)) * row_size*size;	//区码（一个区有94个汉字） * 94,	位码(算出前面的位数)	* 字大小 = 地址
	
	w25_Re_Pg(addr,buff,row_size * size);
	
		for(i=0;i<size;i++)//先循环行数 长
		{
			for(k=0;k<row_size;k++)//决定这一行中哪一字节数据
			{
					for(j=0;j<8;j++)//再循环列 宽
					{
						if(buff[i*row_size + k] & 0x80 >> j) // 第i行 * 每行的字节
						{
							Lcd_Set_One_Position(x+j+8*k,y+i,font_color);// 8 * k 为这一行的第几个字节
						}
						else
						{
							Lcd_Set_One_Position(x+j+8*k,y+i,bg_color);
						}
					}
			}
			
		}
}

/*************************
 * 函 数 名：Lcd_Display_Str(u16 x,u16 y,u16 font_color,u16 bg_color,u8 *str)
 * 函数功能：Lcd显示字符串
 * 参数:u16 x			X轴起始坐标
			  u16 y   	Y轴起始坐标
				u16 font_color 字体颜色
				u16 bg_color 背景颜色
u8 size		字体大小  16:16 * 16 ,24:24 × 24 ,32:32 × 32
				u8 *pic		指向图片数据
 * 返回值：无
 * 备    注：
*************************/
void Lcd_Display_Str(u16 x,u16 y,u16 font_color,u16 bg_color,u8 size,u8 *str)
{
	u8 i=0;
	while(*str != '\0')
	{
		if(*str >= 0xA1)
		{
			Lcd_Display_Chinese_Char(x,y,font_color,bg_color,size,str);
			str += 2;		
			x += size;
			i ++;
		}
		else		//小于0xa1的时候
		{
			Lcd_Display_Char(x,y,font_color,bg_color,size,*str);
			str += 1;
			x += size/2;
			i ++;
		}	
		if(i > 240 - x)
		{
				x = 5;
				y += size;
				i = 0;
		}
	}	
}









