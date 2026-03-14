#ifndef _RTC_H
#define _RTC_H

#include "main.h"

typedef struct{
	u8 year;
	u8 month;
	u8 day;
	u8 weekday;
	
	u8 hour;
	u8 min;
	u8 second;
}RTC_STRUCT;

extern RTC_STRUCT rtc; 

void Rtc_Init(void);
void Rtc_SetDate(u8 year,u8 month,u8 day,u8 weekday);
void Rtc_SetTime(u8 hour,u8 min,u8 second);
void Rtc_GetValue(void);
void Rtc_Analysis_TimeDate(void);
void Rtc_Analysis_Weekday(void);

#endif
