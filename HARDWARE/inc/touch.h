#ifndef _TOUCH_H
#define _TOUCH_H

#include "main.h"

#define tus 5

#define TOUCH_IIC_SCL_H GPIO_SetBits(GPIOA,GPIO_Pin_8)
#define TOUCH_IIC_SCL_L GPIO_ResetBits(GPIOA,GPIO_Pin_8)

#define TOUCH_IIC_SDA_H GPIO_SetBits(GPIOC,GPIO_Pin_9)
#define TOUCH_IIC_SDA_L GPIO_ResetBits(GPIOC,GPIO_Pin_9)

#define TP_RST_H  GPIO_SetBits(GPIOD,GPIO_Pin_5)
#define TP_RST_L   GPIO_ResetBits(GPIOD,GPIO_Pin_5)

#define TP_INT  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1)

#define TOUCH_IIC_SDA_IN (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9))

typedef struct{
	
	u16 x;
	u16 y;
	u8 sta;	//读取手指是否按下
}CST816_STRUCT;

void Touch_IIc_Init(void);

void Touch_IIc_Start(void);
void Touch_IIc_End(void);

void Touch_IIc_Send_Ack(u8 ack);
u8 Touch_IIc_Rec_Ack(void);

void Touch_IIc_Send_Byte(u8 data);
u8 Touch_IIc_Res_Byte(void);

void Touch_Init(void);
void Touch_Wr_Byte(u8 addr,u8 data);
void Touch_IIc_Re_Ct_Byte(u8 addr,u8 *data,u8 len);

void Touch_Coordinates(void);
void Touch_Range(u16 xs,u16 ys,u16 xe,u16 ye);
#endif

