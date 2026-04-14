#ifndef _CAN_H
#define _CAN_H

#include "main.h"


void Can_Config(u8 BS1,u8 BS2,u8 Mode,u16 psc);
u8 Can_Send_Message(u8 *sendmesg,u8 len);
u8 Can_Res_Message(u8 *resmesg);



#endif
