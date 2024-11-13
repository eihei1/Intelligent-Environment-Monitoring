#ifndef __PWM_H
#define __PWM_H	
#include "sys.h"
void pwm_Init(void);
void LED_pwm_set(uint16_t duet);
void LED_pwm_close(void);
void MOTOR_pwm_set(uint16_t duet);
void MOTOR_pwm_close(void);
#endif
