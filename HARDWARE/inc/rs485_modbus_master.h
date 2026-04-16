#ifndef RS485_MODBUS_MASTER_H
#define RS485_MODBUS_MASTER_H

#include "main.h"

#define RS485_SEND	GPIO_SetBits(GPIOE, GPIO_Pin_1);
#define RS485_RESE	GPIO_ResetBits(GPIOE, GPIO_Pin_1);

#define MODBUS_ID 0x01

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
}Modebus;

typedef struct
{
	//接受到从机的缓存
	u16 buff;
	


}Slave;


extern Slave modbus_slave;
extern Modebus modbus_master;
void U4_Modbus_Init(u32 baud);

void ModbusMaster_Transmit(void);
void ModbusMaster_Receive(void);
void RS485_SendData(u8 *ptx,u8 lenth);

#endif

