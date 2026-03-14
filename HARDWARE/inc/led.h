#ifndef _LED_H
#define _LED_H

#include "main.h"

#define LED1_ON		GPIO_ResetBits(GPIOD,GPIO_Pin_12)
#define LED2_ON		GPIO_ResetBits(GPIOD,GPIO_Pin_13)
#define LED3_ON		GPIO_ResetBits(GPIOD,GPIO_Pin_14)

#define LED1_OFF	GPIO_SetBits(GPIOD,GPIO_Pin_12)
#define LED2_OFF 	GPIO_SetBits(GPIOD,GPIO_Pin_13)
#define LED3_OFF	GPIO_SetBits(GPIOD,GPIO_Pin_14)

#define LED1_OVERTURN GPIO_ToggleBits(GPIOD,GPIO_Pin_12)
#define LED2_OVERTURN GPIO_ToggleBits(GPIOD,GPIO_Pin_13)
#define LED3_OVERTURN GPIO_ToggleBits(GPIOD,GPIO_Pin_14)

void Led_Init(void);

#endif
