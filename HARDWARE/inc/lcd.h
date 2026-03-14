#ifndef _LCD_H
#define _LCD_H


#include "main.h"

#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F

#define LCD_SCK_H		GPIO_SetBits(GPIOB,GPIO_Pin_3)
#define LCD_SCK_L		GPIO_ResetBits(GPIOB,GPIO_Pin_3)

#define LCD_SDI_H		GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define LCD_SDI_L		GPIO_ResetBits(GPIOB,GPIO_Pin_5)

#define LCD_CS_H		GPIO_SetBits(GPIOB,GPIO_Pin_4)
#define LCD_CS_L		GPIO_ResetBits(GPIOB,GPIO_Pin_4)

#define LCD_CMD			GPIO_ResetBits(GPIOD,GPIO_Pin_3)
#define LCD_DATA		GPIO_SetBits(GPIOD,GPIO_Pin_3)

#define LCD_BLK_ON		GPIO_SetBits(GPIOD,GPIO_Pin_4)
#define LCD_BLK_OFF		GPIO_ResetBits(GPIOD,GPIO_Pin_4)

extern const unsigned char gImage_car4[153608];
extern const unsigned char ikun[][288];

void lcd_Port_Init(void);
void Lcd_TransferData(u8 data);
void Lcd_SendCmd(u8 data);
void Lcd_Send_Data(u8 data);
void Lcd_SendData_16B(u16 data);
void Lcd_SetPosition(u16 xs,u16 ys,u16 xe,u16 ye);
void LCD_Init(void);
void Lcd_Clear(u16 xs,u16 ys,u16 xe,u16 ye,u16 color);
void Lcd_Set_One_Position(u16 x,u16 y,u16 color);
void Lcd_Display_Photo(u16 x,u16 y,u8 *pic);
void Lcd_Display_Char(u16 x,u16 y,u16 font_color,u16 bg_color,u8 size,u8 eng);
void Lcd_Display_Chinese_Char(u16 x,u16 y,u16 font_color,u16 bg_color,u8 size,u8 *chn);
void Lcd_Display_Str(u16 x,u16 y,u16 font_color,u16 bg_color,u8 size,u8 *str);


#endif



