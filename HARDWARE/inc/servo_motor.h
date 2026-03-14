#ifndef _SERVO_MOTOR_H
#define _SERVO_MOTOR_H

#include "main.h"

void Tim2_ServoMotor_Init(u16 ccr);
void Servo_motor_Control(u8 angle);

#endif

