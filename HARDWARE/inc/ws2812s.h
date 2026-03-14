#ifndef _WS2812S_H
#define _WS2812S_H

#include "main.h"

#define T0H 			Delay_Ns_Rgb()
#define T0L				Delay_Us(1)
#define T1L 			Delay_Ns_Rgb()
#define T1H 			Delay_Us(1)
#define Res_Time	Delay_Us(60)


#define GPIOB_H		GPIO_SetBits(GPIOB,GPIO_Pin_15)
#define GPIOB_L		GPIO_ResetBits(GPIOB,GPIO_Pin_15)

void Rgb_Init(void);
void Rgb_Send_Data(u8 data);
void Rgb_Rest(void);
void Rgb_Control(u8 R,u8 G,u8 B);
void Rgb_Rainbow_Loop(void);
void Rgb_Rainbow_Cycle(u32 delay_ms, u32 steps);

#endif


