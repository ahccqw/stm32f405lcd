#ifndef _STERILIZE_H
#define _STERILIZE_H

#include "main.h"

#define Sterilize_ON	GPIO_SetBits(GPIOE,GPIO_Pin_0)
#define Sterilize_OFF	GPIO_ResetBits(GPIOE,GPIO_Pin_0)

void Sterilize_Init(void);


#endif


