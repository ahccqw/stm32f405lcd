#include "ws2812s.h"

/*************************
函数名称：Rgb_Init(void)
函数功能：Rgb初始化函数 
返回值:无
形参：无
作者：me
版本：1.0 
PB15
*************************/
void Rgb_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	Rgb_Rest();
	Rgb_Control(0,0,0);
	Rgb_Control(0,0,0);
	Rgb_Control(0,0,0);
	Rgb_Control(0,0,0);
	
}

/*************************
函数名称：Rgb_Send_Data(u8 data)
函数功能：Rgb发送字节
返回值:无
形参：无
作者：me
版本：1.0 
*************************/
void Rgb_Send_Data(u8 data)
{
	for(u8 i=0;i<8;i++)
	{
		if(data & (0x80 >> i))
		{
				GPIOB_H;
				T1H;
				GPIOB_L;
				T1L;
		}
		else
		{
				GPIOB_H;
				T0H;
				GPIOB_L;
				T0L;		
		}
	}
}

/*************************
函数名称：Rgb_Rest(void)
函数功能：Rgb置0
返回值:无
形参：无
作者：me
版本：1.0 
*************************/
void Rgb_Rest(void)
{
	GPIOB_L;
	Res_Time;
}

/*************************
函数名称：Rgb_Control(u8 R,u8 G,u8 B)
函数功能：Rgb初始化函数 
返回值:无
形参：无
作者：me
版本：1.0 
*************************/
void Rgb_Control(u8 R,u8 G,u8 B)
{
	Rgb_Send_Data(G);
	Rgb_Send_Data(R);
	Rgb_Send_Data(B);
	
	Rgb_Send_Data(G);
	Rgb_Send_Data(R);
	Rgb_Send_Data(B);
	
	Rgb_Send_Data(G);
	Rgb_Send_Data(R);
	Rgb_Send_Data(B);
	
}

/*************************
函数名称：Rgb_Rainbow_Cycle(u32 delay_ms, u32 steps)
函数功能：四盏灯同步循环全部RGB颜色（HSV色轮）
返回值：无
形参：delay_ms - 每步延时(毫秒), steps - 循环步数(建议256或360)
作者：me
版本：1.0
*************************/
void Rgb_Rainbow_Cycle(u32 delay_ms, u32 steps)
{
    u32 i;
    u8 R, G, B;
    u8 h;          // 色调 0~255 映射到 0°~360°
    u8 region, remainder, q, t;

    for (i = 0; i < steps; i++)
    {
        h = (u8)((i * 255) / steps);  // 色调均匀步进

        // HSV 转 RGB（S=255, V=128 — 亮度适中，防止电流过大）
        region    = h / 43;            // 共6个色区，每区 255/6 ≈ 43
        remainder = (h - (region * 43)) * 6;  // 区内插值 0~255

        q = (128 * (255 - remainder)) >> 8;   // 下降沿
        t = (128 * remainder) >> 8;           // 上升沿

        switch (region)
        {
            case 0:  R = 128; G = t;   B = 0;   break;  // 红→黄
            case 1:  R = q;   G = 128; B = 0;   break;  // 黄→绿
            case 2:  R = 0;   G = 128; B = t;   break;  // 绿→青
            case 3:  R = 0;   G = q;   B = 128; break;  // 青→蓝
            case 4:  R = t;   G = 0;   B = 128; break;  // 蓝→品红
            default: R = 128; G = 0;   B = q;   break;  // 品红→红
        }

        // 四盏灯同色发送
        Rgb_Send_Data(G);
        Rgb_Send_Data(R);
        Rgb_Send_Data(B);

        Rgb_Send_Data(G);
        Rgb_Send_Data(R);
        Rgb_Send_Data(B);

        Rgb_Send_Data(G);
        Rgb_Send_Data(R);
        Rgb_Send_Data(B);

        Rgb_Send_Data(G);
        Rgb_Send_Data(R);
        Rgb_Send_Data(B);

        Rgb_Rest();
        Delay_Ms(delay_ms);
    }
}


