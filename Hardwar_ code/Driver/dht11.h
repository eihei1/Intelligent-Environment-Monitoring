#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   

//�����Ҫ�޸����ţ�ֻ���޸�����ĺ�
#define DHT11_GPIO_PIN      GPIO_Pin_8
#define DHT11_GPIO_PORT     GPIOA

#define DHT11_OUT_0   GPIO_ResetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN)//IOΪ�͵�ƽ
#define DHT11_OUT_1   GPIO_SetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN)//IOΪ�ߵ�ƽ
#define READ_DHT11_IO GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN)//��ȡIO��ƽ

u8 DHT11_Init(void);
u8 DHT11_Read_Data(u8 *Tdata,u8 *RHdata);

#endif















