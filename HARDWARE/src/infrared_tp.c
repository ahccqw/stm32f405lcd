#include "infrared_tp.h"

/*************************
函数名称：infrared_tp(void)
函数功能：红外温度传感器MLX90614
返回值：无
形参：无
作者：me
版本：1.0 	
*************************/
float infrared_tp(void)
{
	u8 lb,mb;
	float temp = 0;
	u16 data = 0;
	
	
	IIc_Start();
	
	IIc_Send_Byte(0xB4);
	IIc_Rec_Ack();
	
	IIc_Send_Byte(0x07);
	IIc_Rec_Ack();

	IIc_Start();
	
	IIc_Send_Byte(0xB5);
	IIc_Rec_Ack();
	
	lb = IIc_Res_Byte();
	IIc_Send_Ack(0);
	
	mb = IIc_Res_Byte();
	IIc_Send_Ack(0);
	
	IIc_Res_Byte();
	IIc_Send_Ack(1);

	IIc_End();
	
	data = (mb << 8) | lb;
	
	temp = (data * 0.02f) - 273.15f;
	
	return temp;
}

