#ifndef _W25Q64_H
#define _W25Q64_H


#include "main.h"

#define W25Q64_CS_H GPIO_SetBits(GPIOC,GPIO_Pin_7)
#define W25Q64_CS_L GPIO_ResetBits(GPIOC,GPIO_Pin_7)

extern u8 Re_flag;
extern u8 Re_End_flag;


void w25_Change_By(void);
void w25_Wr_Enable(void);
u8 w25_Re_Free(void);
void w25_Sector_Erase(u32 addr,u8 cmd);//ÉÈÇø²Á³ý
void w25_Wr_Pg(u32 addr,u8 *data,u32 len);//Ò³Ð´
void w25_Re_Pg(u32 addr,u8 *data,u32 len);//Ò³¶Á
void w25_Wr_Ct(u32 addr,u8 *data,u32 len);//¿çÒ³Ð´
void Font_Updata(void);



#endif
