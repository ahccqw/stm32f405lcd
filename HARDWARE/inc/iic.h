#ifndef _IIC_H
#define _IIC_H

#include "main.h"

#define us 8

#define IIC_SCL_H GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define IIC_SCL_L GPIO_ResetBits(GPIOB,GPIO_Pin_6)

#define IIC_SDA_H GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define IIC_SDA_L GPIO_ResetBits(GPIOB,GPIO_Pin_7)

#define IIC_SDA_IN (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7))

void IIc_Init(void);
void IIc_Start(void);
void IIc_End(void);
void IIc_Send_Ack(u8 ack);
u8 IIc_Rec_Ack(void);
void IIc_Send_Byte(u8 data);
u8 IIc_Res_Byte(void);


#endif

