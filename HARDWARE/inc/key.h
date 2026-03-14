#ifndef _KEY_H
#define _KEY_H

#include "main.h"

#define KEY1_ON (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
#define KEY2_ON (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0))
#define KEY3_ON (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1))

void Key_Init(void);
u8 Key_Scan(void);
 

#endif

