#ifndef MODBUS_SLVE_H
#define MODBUS_SLVE_H

#include "main.h"

#define RS485_SEND	GPIO_SetBits(GPIOE, GPIO_Pin_1);
#define RS485_RESE	GPIO_ResetBits(GPIOE, GPIO_Pin_1);

#define MODBUS_SLVE_ID   0X01

typedef struct
{
	//接收
	u8 deviceaddr;	//设备地址
	u8 rxbuff[256];	//存放接收到的数据
	u8 rxcount;			//接受到数据的个数
	u8 rxover;			//接收完成标志位
	
	//发送
	u8 txbuff[256];	//存放需要发送的数据
	u8 txcount;			//发送数据的个数
}Modebus_slve;

extern Modebus_slve modbus_slve;

void U4_Modbus_Slave_Init(void);
void modbus_slave_analy(void);
void Modbus_Handle_03(void);
void modbus_error(u8 exception_code);
void get_Value(void);


#endif
