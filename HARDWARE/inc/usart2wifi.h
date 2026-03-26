#ifndef _USART2WIFI_H
#define _USART2WIFI_H

#include "main.h"

typedef struct{
	
	u8 buff[256];//保存接收的字符串
	u16 len;//记录接收数据长度的大小
	u8 flag;//记录数据是否完成	0:未接收到新数据		1：接收数据完成
	u8 rec_flag;
	u8 music_handled;
	
}U2;

extern U2 u2;


void Usart2_Init(u32 baud);
void USART2_IRQHandler(void);
void USART2_SendByte(u8 data);
extern void USART2_SendString(char *str);

u8 Esp_SendData(u8 *at,u16 timeout);
void Wifi_Tcp_Init(void);
void Wifi_Control(void);
void Wifi_Send_Message(void);
void Status_Upload(void);


#endif
