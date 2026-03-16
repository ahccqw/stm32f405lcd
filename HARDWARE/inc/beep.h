#ifndef _BEEP_H
#define _BEEP_H

#include "main.h"

#define BEEP_OVERTURN GPIO_ToggleBits(GPIOD,GPIO_Pin_15);

void Beep_Init(void);
void Beep_Start(void);



#endif

