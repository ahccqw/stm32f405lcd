#ifndef _SYSTICK_H
#define _SYSTICK_H

#include "main.h"

extern u8 g_touch_event;
extern u32 g_tick_ms;

void SysTick_Intrerput_Init(u16 nms);
u32 Get_Ms(void);

#endif
