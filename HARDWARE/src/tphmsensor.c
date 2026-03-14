#include "tphmsensor.h"

float TH_Data = 0;
float RH_Data = 0;
/*************************
函数名称：tphm_Init(void)
函数功能：温湿度传感器初始化
返回值：无
形参：无
作者：me
版本：1.0 	
*************************/
void tphm_Init(void)
{
	u8 tp_mb,tp_lb,hm_mb,hm_lb;
	u16 tp_raw, hm_raw; // 用于存储组合后的原始数据
	
	IIc_Start();
	IIc_Send_Byte(0x44 << 1);
	IIc_Rec_Ack();
	
	IIc_Send_Byte(0x2c);
	IIc_Rec_Ack();
	IIc_Send_Byte(0x06);
	IIc_Rec_Ack();
	
	IIc_End();
	Delay_Ms(30);
	
	IIc_Start();
	IIc_Send_Byte((0x44 << 1) | 0x01);
	IIc_Rec_Ack();
	
	tp_mb = IIc_Res_Byte();
	IIc_Send_Ack(0);
	tp_lb = IIc_Res_Byte();
	IIc_Send_Ack(0);
	IIc_Res_Byte(); // 丢弃温度CRC
	IIc_Send_Ack(0); 
	
	hm_mb = IIc_Res_Byte();
	IIc_Send_Ack(0);
	hm_lb = IIc_Res_Byte();
	IIc_Send_Ack(0);
	IIc_Res_Byte(); 
	IIc_Send_Ack(1); 

	IIc_End();
	
	tp_raw = ((u16)tp_mb << 8) | tp_lb;
  hm_raw = ((u16)hm_mb << 8) | hm_lb;
	
	RH_Data = (u32)100 * hm_raw / 65535; 
	TH_Data = (u32)175 * tp_raw / 65535 - 45;
}





