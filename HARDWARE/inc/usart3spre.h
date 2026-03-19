#ifndef _USART3SPRE_H
#define _USART3SPRE_H

#include "main.h"

typedef struct{
	
	u8 buff[10];//保存接收的字符串
	u16 len;//记录接收数据长度的大小
	u8 flag;//记录数据是否完成	0:未接收到新数据		1：接收数据完成
	
}U3;

extern U3 u3;

void Usart3_Init(u32 baud);
void Voice_Control(void);

#endif
