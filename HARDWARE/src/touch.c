#include "touch.h"

CST816_STRUCT cst;
/*************************
函数名称：Touch_IIc_Init(void)
函数功能：IIc触摸初始化配置函数
返回值：无
形参：无
作者：me
版本：1.0 
	IIC3_SCL --- PA8 --- 通用推挽输出
	IIC3_SDA --- PC9 --- 通用开漏输出				AHB1	
	TP_RST	 --- PD5 --- 通用推挽输出				复位输入低电平有效
	TP_TINT	 --- PD1 --- 输入模式						
																					设备地址为：0X15,写入地址为0X2A,读地址为0X2B
*************************/
void Touch_IIc_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOD,&GPIO_InitStruct);

	TOUCH_IIC_SCL_H;
	TOUCH_IIC_SDA_H;
	TP_RST_H;
}

/*************************
函数名称：Touch_IIc_Start(void)
函数功能：Touch_IIc起始信号函数 
返回值：无
形参：无
作者：me
版本：1.0 
	在时钟线为高电平的时候去拉低数据线
*************************/
void Touch_IIc_Start(void)
{
	TOUCH_IIC_SDA_H;
	TOUCH_IIC_SCL_H;
	Delay_Us(us);
	TOUCH_IIC_SDA_L;
	Delay_Us(us);
	
	
}

/*************************
函数名称：Touch_IIc_End(void)
函数功能：Touch_IIc结束信号函数 
返回值：无
形参：无
作者：me
版本：1.0 
	在时钟线为高电平的时候去拉高数据线
*************************/
void Touch_IIc_End(void)
{
	TOUCH_IIC_SDA_L;
	TOUCH_IIC_SCL_H;
	Delay_Us(us);
	TOUCH_IIC_SDA_H;
	Delay_Us(us);
}

/*************************
函数名称：Touch_IIc_Send_Ack(u8 ack)
函数功能：Touch_IIc发送应答/不应答信号函数 
返回值：无
形参：u8 ack		0 发送应答 	1 发送不应答
作者：me
版本：1.0 
*************************/
void Touch_IIc_Send_Ack(u8 ack)
{
	TOUCH_IIC_SCL_L;
	Delay_Us(us);
	if(ack == 1)
	{
		TOUCH_IIC_SDA_H;
	}
	else
	{
		TOUCH_IIC_SDA_L;
	}
	Delay_Us(us);
	
	TOUCH_IIC_SCL_H;
	Delay_Us(us);
	
	TOUCH_IIC_SCL_L;
	Delay_Us(us);
}

/*************************
函数名称：Touch_IIc_Rec_Ack(void)
函数功能：Touch_IIc接收应答/不应答信号函数 
返回值：u8 ack		0 表示应答 	1 表示不应答
形参：无
作者：me
版本：1.0 				SCL推挽		SDA开漏
*************************/
u8 Touch_IIc_Rec_Ack(void)
{
	u8 ack;
	
	TOUCH_IIC_SDA_H;
	
	TOUCH_IIC_SCL_L;//拉低时钟线，从机向主机发送1bit应答
	Delay_Us(us);
	TOUCH_IIC_SCL_H;//拉高时钟线，主机接收刚刚从机发送的1bit应答
	Delay_Us(us);
	if(TOUCH_IIC_SDA_IN)
	{
		ack = 1;
	}
	else
	{
		ack = 0;
	}
	Delay_Us(us);
	
	
	TOUCH_IIC_SCL_L;
	Delay_Us(us);
	return ack;
}

/*************************
函数名称：Touch_IIc_Send_Byte(u8 data)
函数功能：Touch_IIc发送一个字节
返回值：无
形参：u8 data
作者：me
版本：1.0 
*************************/
void Touch_IIc_Send_Byte(u8 data)
{
	for(u8 i=0;i<8;i++)
	{
		TOUCH_IIC_SCL_L;
		Delay_Us(us);
		if(data & (0x80 >> i))
		{
			TOUCH_IIC_SDA_H;	
		}
		else
		{
			TOUCH_IIC_SDA_L;	
		}
		Delay_Us(us);
		TOUCH_IIC_SCL_H;
		Delay_Us(us);
	}
	
}

/*************************
函数名称：Touch_IIc_Res_Byte(void)
函数功能：Touch_IIc接收一个字节
返回值：u8 
形参：无
作者：me
版本：1.0 
		数据是从高位开始发
*************************/
u8 Touch_IIc_Res_Byte(void)
{
	u8 data;
	TOUCH_IIC_SDA_H;     // ? 释放SDA，让传感器能驱动总线
	for(u8 i=0;i<8;i++)
	{
		
		TOUCH_IIC_SCL_L;//拉低时钟线，从机向主机发送1bit数据
		Delay_Us(us);
		TOUCH_IIC_SCL_H;//拉高时钟线，从机向主机发送1bit数据
		Delay_Us(us);
		
		data <<= 1;
		if(TOUCH_IIC_SDA_IN)
		{
			data |= 1;
		}
		Delay_Us(us);
		
		
	}
	
	return data;
}

/*************************
函数名称：Touch_Init(void)
函数功能：Touch初始化
返回值：无 
形参：无
作者：me
版本：1.0 
*************************/
void Touch_Init(void)
{
	u8 id=0;
	Touch_IIc_Init();
	TP_RST_L;
	Delay_Ms(50);
	TP_RST_H;
	Delay_Ms(50);
	Touch_Wr_Byte(0xFD,0);
	Touch_IIc_Re_Ct_Byte(0xA7,&id,1);
	printf("ChipID:0x%02x\r\n",id);

}


/*************************
函数名称：Touch_Wr_Byte(void)
函数功能：IIc写入一个字节
返回值：无 
形参：无
作者：me
版本：1.0 
		数据是从高位开始发
*************************/
void Touch_Wr_Byte(u8 addr,u8 data)
{
	Touch_IIc_Start();
	Touch_IIc_Send_Byte(0X2A);
	Touch_IIc_Rec_Ack();
	Touch_IIc_Send_Byte(addr);
	Touch_IIc_Rec_Ack();
	Touch_IIc_Send_Byte(data);
	Touch_IIc_Rec_Ack();
	Touch_IIc_End();
}

/*************************
函数名称：Touch_IIc_Res_Byte(u8 addr,u8 *data,u32 len)
函数功能：IIc读取多个字节
返回值：无 
形参：u8 addr,u8 *data,u32 len
作者：me
版本：1.0 
		数据是从高位开始发
*************************/
void Touch_IIc_Re_Ct_Byte(u8 addr,u8 *data,u8 len)
{

	Touch_IIc_Start();
	Touch_IIc_Send_Byte(0X2A);
	Touch_IIc_Rec_Ack();
	Touch_IIc_Send_Byte(addr);
	Touch_IIc_Rec_Ack();
	Touch_IIc_Start();
	Touch_IIc_Send_Byte(0X2B);
	Touch_IIc_Rec_Ack();

	len-=1;
	while(len--)
	{

		*data = Touch_IIc_Res_Byte();
		data++;
		Touch_IIc_Send_Ack(0);
	}
	*data = Touch_IIc_Res_Byte();
	Touch_IIc_Send_Ack(1);

	Touch_IIc_End();
}

	//假设你要读取 3 个字节 (len = 3)：
	//执行 len -= 1，此时 len 变为 2。
	//进入 while(len--) 循环：
	//第 1 次循环：读取第 1 个字节，发送 ACK。(len 变为 1)
	//第 2 次循环：读取第 2 个字节，发送 ACK。(len 变为 0，循环结束)
	//跳出循环，执行后续代码：
	//读取 第 3 个字节（最后一个）。
	//发送 NACK (Ack(1))。


/*************************
函数名称：Touch_Coordinates(void)
函数功能：Touch发送触摸的位置
返回值：无 
形参：无
作者：me
版本：1.0 
*************************/
void Touch_Coordinates(void)
{
	u8 temp[4] = {0};
	u8 flag = 0xff;
	if(TP_INT == 0)
	{
		Touch_IIc_Re_Ct_Byte(0x01,&flag,1); 
		
		if(flag != 0x00 && flag <= 0x05)
		{
			cst.sta = flag;
			printf("sta: %d\r\n",cst.sta);
			
			Touch_IIc_Re_Ct_Byte(0x03,temp,4);
			
			cst.x = (temp[0] & 0x0f) << 8 | temp[1];
			cst.y = (temp[2] & 0x0f) << 8 | temp[3];
			printf("x:%d y:%d \r\n",cst.x,cst.y);
			printf("sta: %d\r\n",cst.sta);
		}
		else
		{
			cst.sta = 0x00;
		}
	}
	else
	{
		cst.sta = 0x00;
	}
	
}

/*************************
函数名称：Touch_Range_funtion(u16 xs,u16 ys,u16 xe,u16 ye)
函数功能：Touch触发的范围
返回值：无 
形参：无
作者：me
版本：1.0 
*************************/
void Touch_Range(u16 xs,u16 ys,u16 xe,u16 ye)
{
	// 静态变量保存当前页面和LED状态，避免重复刷新
	static u8 current_page = 0;
	static u8 state = 0;
	static u8 motor_state = 0;
	static u8 servo_state = 0;
	static u8 last_page = 0xFF;  // 记录上次页面
	
	// 触摸状态标志，防止一次触摸多次触发
	static u8 touch_processed = 0;

	u8 in_range = ((cst.x>xs && xe>cst.x) && (cst.y>ys && cst.y<ye));
	
	if(in_range && cst.sta != 0x00 && touch_processed == 0)
	{
		// 只有有效触摸且未处理过才响应
		if((cst.x>xs && xe>cst.x) && (cst.y>ys && cst.y<ye) && (cst.sta == 0x03))
		{	
			touch_processed = 1;
			
			// 左滑 0x03
			if(cst.sta == 0x03)
			{
					current_page = (current_page == 0) ? 3 : (current_page - 1);
			}
			// 右滑 0x04
			else if(cst.sta == 0x04)
			{
					current_page = (current_page == 3) ? 0 : (current_page + 1);
			}
			// 单击 0x05
			else if(cst.sta == 0x05)
			{
					state = !state;  // 切换状态
			}
			
			// 清除触摸状态，防止重复触发
			cst.sta = 0x00;
			cst.x = 0xFFFF;
			cst.y = 0xFFFF;		
		}
		
		 // 无触摸时重置处理标志
    if(TP_INT != 0)
    {
        touch_processed = 0;
    }
		
		    // ================= 页面显示（只在页面变化时刷新）=================
    static u8 last_page = 0xFF;  // 记录上次页面
    
    if(current_page != last_page)
    {
        Lcd_Clear(xs, ys, xe, ye, WHITE);  // 只在换页时清屏
        last_page = current_page;
    }
		
		
		 // 根据当前页面显示内容
    switch(current_page)
    {
        case 0:  // 页面0：白色背景
            // 无需操作，清屏已是白色
            break;
            
        case 1:  // 页面1：黑色背景 + LED控制
            Lcd_Clear(xs, ys, xe, ye, BLACK);
            last_page = current_page;
            if(state)
            {
                LED1_ON; LED2_ON; LED3_ON;
            }
            else
            {
                LED1_OFF; LED2_OFF; LED3_OFF;
            }
            break;
            
        case 2:  // 页面2：黑色背景 + 直流电机
            Lcd_Clear(xs, ys, xe, ye, BLACK);
            last_page = current_page;
            if(state)  // 复用led_state控制电机
            {
                Tim3_DcMotor_Init(500);
            }
            else
            {
                Tim3_DcMotor_Init(0);
            }
            break;
            
        case 3:  // 页面3：黑色背景 + 舵机
            Lcd_Clear(xs, ys, xe, ye, BLACK);
            last_page = current_page;
            if(state)  // 复用led_state控制舵机
            {
                Tim2_ServoMotor_Init(500);
            }
            else
            {
                Tim2_ServoMotor_Init(1000);
            }
            break;
            
        default:
            current_page = 0;
            break;
    }	
	}
}










