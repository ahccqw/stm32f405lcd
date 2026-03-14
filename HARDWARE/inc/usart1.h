#ifndef _USART1_H
#define _USART1_H

#include "main.h"

typedef struct{
	
	u8 buff[1024];//保存接收的字符串
	u16 len;//记录接收数据长度的大小
}U1;

extern U1 u1;



void Usart1_Init(u32 baud);
void Usart1_SendByte(u8 data);
void Usart1_SendString(char *str);
int fputc(int c, FILE * stream);
u8 Usart1_RecByte(void);

#endif

