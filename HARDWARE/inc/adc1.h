#ifndef _LIGHT_SENSOR_H
#define _LIGHT_SENSOR_H

#include "main.h"

void ADC1_Init(void);
u16 Light_Sensor_GetVal(void);
u16 Fire_Sensor_GetVal(void);

#endif


