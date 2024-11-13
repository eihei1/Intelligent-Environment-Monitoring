#ifndef __GPIO_H
#define __GPIO_H	 
#include "sys.h" 

#define KEY1 PBin(12)
#define KEY2 PBin(13)
#define KEY3 PBin(14)

#define BEEP   PCout(15) 
#define RELAY  PBout(9) 
#define MOTOR  PBout(8) 
#define LED    PBout(7) 

void KEY_GPIO_Init(void);//Òý½Å³õÊ¼»¯
void MOTOR_GPIO_Init(void);

#endif
