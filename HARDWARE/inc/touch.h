#ifndef _TOUCH_H
#define _TOUCH_H

#include "main.h"

#define tus 5

#define TOUCH_IIC_SCL_H GPIO_SetBits(GPIOA,GPIO_Pin_8)
#define TOUCH_IIC_SCL_L GPIO_ResetBits(GPIOA,GPIO_Pin_8)

#define TOUCH_IIC_SDA_H GPIO_SetBits(GPIOC,GPIO_Pin_9)
#define TOUCH_IIC_SDA_L GPIO_ResetBits(GPIOC,GPIO_Pin_9)

#define TP_RST_H  GPIO_SetBits(GPIOD,GPIO_Pin_5)
#define TP_RST_L   GPIO_ResetBits(GPIOD,GPIO_Pin_5)

#define TP_INT  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1)

#define TOUCH_IIC_SDA_IN (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9))

typedef struct{
	
	u16 x;
	u16 y;
	u8 sta;	//读取手指是否按下
	
	 //功能状态变量
	u8 led_state;           // LED状态 0-关 1-开
	u8 motor_state;         // 直流电机状态 0-关 1-开
	u8 servo_state;         // 舵机状态 0-关 1-开
	u8  rgb_state;          // RGB开关状态 0-关 1-开
	u8 Germicidal_state;
	
	
	u8 last_led_state;      // 上次LED状态
	u8 last_motor_state;    // 上次电机状态
	u8 last_servo_state;    // 上次舵机状态
	u8 last_rgb_upload_state;  // 上次上传的RGB开关状态
	u8 last_germicidal_upload;
	
	u8 first_upload;
	
	//页面状态变量
	u8 current_page;        // 当前页面
	u8 last_page;           // 上次页面
	
	//音乐
	u8 previous_song_state;
	u8 song_start_flag;
	u8 next_song_state;
	
	u8 last_previous_song_state;
	u8 last_song_start_flag;
	u8 last_next_song_state;
	
	//触发对应范围的标志位 
	u8 rang_flag;
	
 // ===== 新增：上传用的上次状态 =====
	u8 last_led_upload;
	u8 last_motor_upload;
	u8 last_servo_upload;

	// ===== 新增：有变化待上传标志 =====
	u8 need_upload;
	
	// ===== 记录数值 =====
	u32 rgb_value;          // 当前RGB值
	u16 motor_value;
	u32 last_rgb_upload;    // 上次上传的RGB值
	
	
	
}CST816_STRUCT;

extern CST816_STRUCT cst;



void Touch_IIc_Init(void);

void Touch_IIc_Start(void);
void Touch_IIc_End(void);

void Touch_IIc_Send_Ack(u8 ack);
u8 Touch_IIc_Rec_Ack(void);

void Touch_IIc_Send_Byte(u8 data);
u8 Touch_IIc_Res_Byte(void);

void Touch_Init(void);
void Touch_Wr_Byte(u8 addr,u8 data);
void Touch_IIc_Re_Ct_Byte(u8 addr,u8 *data,u8 len);

void Touch_Coordinates(void);
void Touch_Range(u16 xs,u16 ys,u16 xe,u16 ye);

void Device_State_Update(void);
#endif

