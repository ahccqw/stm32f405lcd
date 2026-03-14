#include "rtc.h"

u8 time[] = __TIME__;//系统时间（以字符串的形式表示）
u8 date[] = __DATE__;//系统日期                 

RTC_STRUCT rtc;
/*************************
函数名称：Rtc_Init(void)
函数功能：Rtc初始化函数
返回值:无
形参：无
作者：me
版本：1.0 
*************************/
void Rtc_Init(void)
{
	//打开PWR的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	PWR_BackupAccessCmd(ENABLE);//DBP置1 解除备份域 写保护
	RTC_WriteProtectionCmd(DISABLE);//关闭RTC寄存器写保护
	
	//打开LSE时钟
	RCC_LSEConfig(RCC_LSE_ON);
	if(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == SET)
	{
			//如LSE标志位为1，则继续完成初始化
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//选择LSE作为RTC时钟源
		RCC_RTCCLKCmd(ENABLE);//使能RTC时钟
		
		RTC_InitTypeDef RTC_InitStruct = {0};
		RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;//小时制
		RTC_InitStruct.RTC_AsynchPrediv = 128 -1;//异步分频		
		RTC_InitStruct.RTC_SynchPrediv = 256 - 1;//同步分频	
		RTC_Init(&RTC_InitStruct);
		
		//解析当前的时间和日期
		Rtc_Analysis_TimeDate();
		//解析星期
		Rtc_Analysis_Weekday();
		//设置日期
		Rtc_SetDate(rtc.year,rtc.month,rtc.day,rtc.weekday);
		//设置时间
		Rtc_SetTime(rtc.hour,rtc.min,rtc.second);
	
	}
	else
	{
			//如LSE标志位为0，则LSE打开失败
		printf("LSE开启失败，请重新上电");
	}
}

/*************************
函数名称：Rtc_SetDate(u8 year,u8 month,u8 day,u8 weekday)
函数功能：Rtc设置日期
返回值:无
形参：无
作者：me
版本：1.0 
	u8 year 0 - 99 
	u8 month 1 - 12
	u8 day 
	u8 weekday
*************************/
void Rtc_SetDate(u8 year,u8 month,u8 day,u8 weekday)
{
	RTC_DateTypeDef RTC_DateStruct;
	RTC_DateStruct.RTC_Date = day;
	RTC_DateStruct.RTC_Month = month;
	RTC_DateStruct.RTC_WeekDay = weekday;
	RTC_DateStruct.RTC_Year = year;
	RTC_SetDate(RTC_Format_BIN,&RTC_DateStruct);
}	

/*************************
函数名称：Rtc_SetTime(u8 hour,u8 min,u8 second)
函数功能：Rtc设置时间
返回值:无
形参：无
作者：me
版本：1.0 
*************************/
void Rtc_SetTime(u8 hour,u8 min,u8 second)
{
	
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_TimeStruct.RTC_Hours = hour;
	RTC_TimeStruct.RTC_Minutes = min;
	RTC_TimeStruct.RTC_Seconds = second;
	RTC_SetTime(RTC_Format_BIN,&RTC_TimeStruct);
}

/*************************
函数名称：Rtc_GetValue(void)
函数功能：Rtc获取实时日期和时间函数
返回值:无
形参：无
作者：me
版本：1.0 
*************************/
void Rtc_GetValue(void)
{
	RTC_DateTypeDef RTC_DateStruct = {0};	
	RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);

	RTC_TimeTypeDef RTC_TimeStruct = {0};
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);

	rtc.year    = RTC_DateStruct.RTC_Year;
	rtc.month   = RTC_DateStruct.RTC_Month;
	rtc.day     = RTC_DateStruct.RTC_Date;
	rtc.weekday = RTC_DateStruct.RTC_WeekDay;
	
	rtc.hour    = RTC_TimeStruct.RTC_Hours;
	rtc.min     = RTC_TimeStruct.RTC_Minutes;
	rtc.second  = RTC_TimeStruct.RTC_Seconds;
	
	printf("data: 20%02d-%02d-%02d Weekday:%d\r\n",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date,RTC_DateStruct.RTC_WeekDay);
	printf("time: %02d:%02d:%02d\r\n",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
}

/*************************
函数名称：Rtc_Analysis_TimeDate(void)
函数功能：解析时间和日期
返回值:无
形参：无
作者：me
版本：1.0 
*************************/
void Rtc_Analysis_TimeDate(void)
{
	u8 i=0;
	u8 temp[4] = {0};
	u8 array[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	//解析小时 字符转十进制:- 48
	rtc.hour = (time[0] - 48) * 10 + (time[1] - 48);
	//解析分钟
	rtc.min = (time[3] - 48) * 10 + (time[4] - 48);
	//解析秒
	rtc.second = (time[6] - 48) * 10 + (time[7] - 48);
	//解析年份
	rtc.year = (date[9] - 48) * 10 + (date[10] - 48);
	//解析月份
	for(i=0;i<3;i++)
	{
		temp[i] += date[i];
	}
	temp[3] = '\0';
	for(i=0;i<12;i++)
	{
		if(strcmp((const char*)temp,(const char*)array[i]) == 0)
		{
			break;
		}
	}
	rtc.month = i+1;
	
	//解析日期
	if(date[4]<'0' || date[4]> '9')
	{
		
		date[4] = '0';
	}
	rtc.day = (date[4] - 48) * 10 + (date[5] - 48);
}

/*************************
函数名称：Leap_Year(u16 year)
函数功能：判断年份是闰年还是平年
返回值:无
形参：无
作者：me
版本：1.0 
*************************/
unsigned char Leap_Year(u16 year)
{
    if(((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
    {
        //闰年 今年天数+1天
        return 1;
    }
    return 0;
}

/*************************
函数名称：Rtc_Analysis_Weekday(void)
函数功能：解析星期
返回值:无
形参：无
作者：me
版本：1.0 
*************************/
void Rtc_Analysis_Weekday(void)
{
	u16 i;
	u32 day_count = 0;
	u8 months_day[13] = {0,31,28,31,30,31,30,31,30,31,30,31,30};
	//相隔多少年
	for(i=2024;i<rtc.year+2000;i++)
	{
		day_count += 365 + Leap_Year(i);
	}
	
	if(Leap_Year(rtc.year+2000) == 1)
	{
		months_day[2] += 1;
	}
	
	//今年过了多少个月,每个月有几天,判断今年的2月是否为闰年
	for(i=0;i<rtc.month;i++)
	{
		day_count += months_day[i];
	}
		day_count += rtc.day;
	//计算星期几
	switch(day_count % 7)
	{
		case 1:rtc.weekday = 1;break;
		case 2:rtc.weekday = 2;break;
		case 3:rtc.weekday = 3;break;
		case 4:rtc.weekday = 4;break;
		case 5:rtc.weekday = 5;break;
		case 6:rtc.weekday = 6;break;
		case 0:rtc.weekday = 7;break;
	}
}


