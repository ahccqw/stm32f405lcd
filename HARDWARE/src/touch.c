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
	static u8 first_run = 1;  // 首次运行标志 为了能运行 首页
	
	u8 in_range = ((cst.x>xs && xe>cst.x) && (cst.y>ys && cst.y<ye));
	
	static u8	touch_processed = 0;
	
	if(in_range && (cst.sta != 0x00) && (touch_processed == 0))
	{
		 static u8	touch_processed = 1;
			
			switch(cst.sta)
			{
				//左滑
				case 0x03:cst.current_page = (cst.current_page == 0) ? 3 : (cst.current_page - 1);break;//	条件 ? 	结果A : 结果B。
				//右滑
				case 0x04:cst.current_page = (cst.current_page == 3) ? 0 : (cst.current_page + 1);break;
				case 0x05:
					switch(cst.current_page)
					{
							case 1: cst.led_state = !cst.led_state; break;
							case 2: cst.motor_state = !cst.motor_state; break;
							case 3: cst.servo_state = !cst.servo_state; break;
							default: break;
					}
					break;
					
				default:break;
					
			}		
			// 清除触摸状态，防止重复触发
			cst.sta = 0x00;
			cst.x = 0xFFFF;
			cst.y = 0xFFFF;		
	}	
		 // 手指抬起（中断脚为高）后，允许下一次触摸再次触发
    if(TP_INT != 0)
    {
        touch_processed = 0;
    }
		
		    // ================= 页面显示（只在页面变化时刷新）=================
    if((first_run || cst.current_page != cst.last_page) ||    (cst.led_state != cst.last_led_state) || (cst.motor_state != cst.last_motor_state) || (cst.servo_state != cst.last_servo_state))
    {
				first_run = 0;//进来之后直接 赋0
			
        cst.last_page = cst.current_page;	
        cst.last_led_state = cst.led_state;
        cst.last_motor_state = cst.motor_state;
        cst.last_servo_state = cst.servo_state;
			 // 根据当前页面显示内容
			
			switch(cst.current_page)
			{
					case 0:  // 页面0：主页面
							Lcd_Display_Photo(xs,ys,(u8 *)gImage_0page);
							break;
							
					case 1:  // 页面1：黑色背景 + LED控制
							Lcd_Clear(xs, ys, xe, ye, WHITE);
							
							if(cst.led_state)
							{		
									Lcd_Display_Photo(60,80,(u8 *)gImage_11page);
									LED1_ON; LED2_ON; LED3_ON;
									Rgb_Control(0xff,0xff,0xff);
									Rgb_Control(0xff,0xff,0xff);
									Rgb_Control(0xff,0xff,0xff);
									Rgb_Control(0xff,0xff,0xff);
							}
							else
							{
									Lcd_Display_Photo(60,80,(u8 *)gImage_1page);
									LED1_OFF; LED2_OFF; LED3_OFF;
									Rgb_Control(0x00,0x00,0x00);
									Rgb_Control(0x00,0x00,0x00);
									Rgb_Control(0x00,0x00,0x00);
									Rgb_Control(0x00,0x00,0x00);
							}
							break;
							
					case 2:  // 页面2：黑色背景 + 直流电机
							Lcd_Clear(xs, ys, xe, ye, WHITE);
							
							if(cst.motor_state)  // 复用led_state控制电机
							{
									Lcd_Display_Photo(60,80,(u8 *)gImage_22page);
									TIM_SetCompare3(TIM3,1000);

							}
							else
							{
									Lcd_Display_Photo(60,80,(u8 *)gImage_2page);
									TIM_SetCompare3(TIM3,0);
							}
							break;
							
					case 3:  // 页面3：黑色背景 + 舵机
							Lcd_Clear(xs, ys, xe, ye, WHITE);
							
							if(cst.servo_state)  // 复用led_state控制舵机
							{
									Lcd_Display_Photo(60,80,(u8 *)gImage_33page);
									Servo_motor_Control(30);   // 比如 30°，对应较小脉宽
							}
							else
							{
									Lcd_Display_Photo(60,80,(u8 *)gImage_3page);
									Servo_motor_Control(120);
							}
							break;
							
					default:
							cst.current_page = 0;
              cst.last_page = 0xff;
							break;
			}	
		}
}










