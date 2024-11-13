#include "sys.h"
#include "delay.h"
#include "adc.h"
#include "gpio.h"
#include "OLED_I2C.h"
#include "timer.h"
#include "dht11.h"
#include "kalman_filter.h"
#include "pwm.h"
#include "esp8266.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
char display[16];
unsigned char setn = 0;//��¼���ü����µĴ���
unsigned char temperature = 0;
unsigned char humidity = 0; //ʪ��
unsigned char setTempValue = 35;        //�¶�����ֵ
unsigned int  light = 0; //����ǿ��
unsigned int  setSoilMoisture = 50;       //����ʪ������ֵ
unsigned int  soilMoisture;           //����ʪ��
unsigned int setLightValue = 20;       //��������ֵ
unsigned int ledduty = 0, motorduty = 0;
unsigned int ledduty_t = 0, motorduty_t = 0;
bool led_flage = 1, motor_flage = 1;//1���� 0����
kalman light_kalman;
kalman soilMoisture_kalman;
int8_t n = 0;
bool shuaxin = 0;
bool shanshuo = 0;
bool sendFlag = 1;

void InitDisplay(void)   //��ʼ����ʾ
{
	unsigned char i = 0;
	for (i = 0;i < 4;i++)OLED_ShowCN(i * 16, 0, i + 0, 0);//��ʾ���ģ������¶�
	for (i = 0;i < 4;i++)OLED_ShowCN(i * 16, 2, i + 4, 0);//��ʾ���ģ�����ʪ��
	for (i = 0;i < 4;i++)OLED_ShowCN(i * 16, 4, i + 8, 0);//��ʾ���ģ�����ʪ��
	for (i = 0;i < 4;i++)OLED_ShowCN(i * 16, 6, i + 12, 0);//��ʾ���ģ�����ǿ��
	OLED_ShowChar(64, 0, ':', 2, 0);
	OLED_ShowChar(64, 2, ':', 2, 0);
	OLED_ShowChar(64, 4, ':', 2, 0);
	OLED_ShowChar(64, 6, ':', 2, 0);
}

void displayDHT11TempAndHumi(void)  //��ʾ������ʪ��
{
	DHT11_Read_Data(&temperature, &humidity);
	if (temperature >= setTempValue && shanshuo)
	{
		OLED_ShowChar(80, 0, ' ', 2, 0);
		OLED_ShowChar(88, 0, ' ', 2, 0);
	}
	else
	{
		OLED_ShowChar(80, 0, temperature / 10 + '0', 2, 0);
		OLED_ShowChar(88, 0, temperature % 10 + '0', 2, 0);
	}
	OLED_ShowCentigrade(96, 0);
	OLED_ShowChar(80, 2, humidity / 10 + '0', 2, 0);
	OLED_ShowChar(88, 2, humidity % 10 + '0', 2, 0);
	OLED_ShowChar(96, 2, '%', 2, 0);
}

void displayLight(void)//��ʾ��ǿ��
{
	u16 test_adc = 0;
	/////////////��ȡ����ֵ
	test_adc = Get_Adc_Average(ADC_Channel_9, 10);//��ȡͨ��9��10��ADƽ��ֵ
	light = (unsigned int)kalman_filter(&light_kalman, (float)(test_adc * 99.0)) / 4096;//ת����0-99�ٷֱ�
	light = light >= 99 ? 99 : light;//���ֻ�ܵ��ٷ�֮99
	if (light <= setLightValue && shanshuo)
	{
		OLED_ShowChar(80, 6, ' ', 2, 0);
		OLED_ShowChar(88, 6, ' ', 2, 0);
	}
	else
	{
		OLED_ShowChar(80, 6, light / 10 + '0', 2, 0);
		OLED_ShowChar(88, 6, light % 10 + '0', 2, 0);
	}
	OLED_ShowChar(96, 6, '%', 2, 0);
}

void displaySoilMoisture(void)//��ʾ����ʪ��
{
	soilMoisture = (unsigned int)(kalman_filter(&soilMoisture_kalman, (float)(Get_Adc_Average(ADC_Channel_8, 10) * 99.0)) / 4096);//ת����0-99�ٷֱ�
	if (soilMoisture > 99)soilMoisture = 99;
	if (soilMoisture >= setSoilMoisture && shanshuo)
	{
		OLED_ShowChar(80, 4, ' ', 2, 0);
		OLED_ShowChar(88, 4, ' ', 2, 0);
	}
	else
	{
		OLED_ShowChar(80, 4, soilMoisture / 10 + '0', 2, 0);
		OLED_ShowChar(88, 4, soilMoisture % 10 + '0', 2, 0);
	}
	OLED_ShowChar(96, 4, '%', 2, 0);
}

void displaySetValue(void)  //��ʾ���õ�ֵ
{
	if (setn == 1)
	{
		OLED_ShowChar(60, 4, setSoilMoisture / 10 + '0', 2, 0);
		OLED_ShowChar(68, 4, setSoilMoisture % 10 + '0', 2, 0);
		OLED_ShowChar(76, 4, '%', 2, 0);
	}
	if (setn == 2)
	{
		OLED_ShowChar(60, 4, setTempValue / 10 + '0', 2, 0);
		OLED_ShowChar(68, 4, setTempValue % 10 + '0', 2, 0);
		OLED_ShowCentigrade(76, 4);
	}
	if (setn == 3)
	{
		OLED_ShowChar(60, 4, setLightValue / 10 + '0', 2, 0);
		OLED_ShowChar(68, 4, setLightValue % 10 + '0', 2, 0);
		OLED_ShowChar(76, 4, '%', 2, 0);
	}
}

void keyscan(void)   //����ɨ��
{
	u8 i;

	if (KEY1 == 0) //����
	{
		delay_ms(20);
		if (KEY1 == 0)
		{
			while (KEY1 == 0);
			BEEP = 0;
			setn++;
			if (setn == 1)
			{
				OLED_CLS();    //����
				for (i = 0;i < 2;i++)
				{
					OLED_ShowCN(i * 16 + 16, 0, i + 16, 0);  //��ʾ����
				}
				for (i = 0;i < 4;i++)OLED_ShowCN(i * 16 + 48, 0, i + 8, 0);//��ʾ���ģ�����ʪ��
			}
			if (setn == 2)
			{
				for (i = 0;i < 4;i++)OLED_ShowCN(i * 16 + 48, 0, i + 0, 0);//��ʾ���ģ������¶�
			}
			if (setn == 3)
			{
				for (i = 0;i < 4;i++)OLED_ShowCN(i * 16 + 48, 0, i + 12, 0);//��ʾ���ģ�����ǿ��
				OLED_ShowChar(84, 4, ' ', 2, 0);
			}
			displaySetValue();
			if (setn > 3)
			{
				setn = 0;
				OLED_CLS();    //����
				InitDisplay();
			}
		}
	}
	if (KEY2 == 0) //��
	{
		delay_ms(20);
		if (KEY2 == 0)
		{
			while (KEY2 == 0);
			if (setn == 1)
			{
				if (setSoilMoisture < 99)setSoilMoisture++;
			}
			if (setn == 2)
			{
				if (setTempValue < 99)setTempValue++;
			}
			if (setn == 3)
			{
				if (setLightValue < 99)setLightValue++;
			}
			displaySetValue();
		}
	}
	if (KEY3 == 0) //��
	{
		delay_ms(20);
		if (KEY3 == 0)
		{
			while (KEY3 == 0);
			if (setn == 1)
			{
				if (setSoilMoisture > 0)setSoilMoisture--;
			}
			if (setn == 2)
			{
				if (setTempValue > 0)setTempValue--;
			}
			if (setn == 3)
			{
				if (setLightValue > 0)setLightValue--;
			}
			displaySetValue();
		}
	}
}

void UsartSendReceiveData(void)
{
	unsigned char* dataPtr = NULL;
	char* str1 = 0, i;
	int  setValue = 0;
	char setvalue[5] = { 0 };
	char SEND_BUF[200];

	dataPtr = ESP8266_GetIPD(0);   //��������
	if (dataPtr != NULL)
	{
		if (strstr((char*)dataPtr, "light:") != NULL)
		{
			BEEP = 1;
			delay_ms(80);
			BEEP = 0;

			str1 = strstr((char*)dataPtr, "light:");

			while (*str1 < '0' || *str1 > '9')    //�ж��ǲ���0��9��Ч����
			{
				str1 = str1 + 1;
			}
			i = 0;
			while (*str1 >= '0' && *str1 <= '9')        //�ж��ǲ���0��9��Ч����
			{
				setvalue[i] = *str1;
				i++; str1++;
			}
			setValue = atoi(setvalue);
			setLightValue = setValue;    //���õĹ���ֵ
			displaySetValue();
		}

		if (strstr((char*)dataPtr, "temp:") != NULL)
		{
			str1 = strstr((char*)dataPtr, "temp:");

			while (*str1 < '0' || *str1 > '9')        //�ж��ǲ���0��9��Ч����
			{
				str1 = str1 + 1;
			}
			i = 0;
			while (*str1 >= '0' && *str1 <= '9')        //�ж��ǲ���0��9��Ч����
			{
				setvalue[i] = *str1;
				i++; str1++;
			}
			setValue = atoi(setvalue);
			setTempValue = setValue;    //���õ��¶�ֵ
			displaySetValue();
		}

		if (strstr((char*)dataPtr, "soil_hmd:") != NULL)
		{
			str1 = strstr((char*)dataPtr, "soil_hmd:");

			while (*str1 < '0' || *str1 > '9')        //�ж��ǲ���0��9��Ч����
			{
				str1 = str1 + 1;
			}
			i = 0;
			while (*str1 >= '0' && *str1 <= '9')        //�ж��ǲ���0��9��Ч����
			{
				setvalue[i] = *str1;
				i++; str1++;
			}
			setValue = atoi(setvalue);
			setSoilMoisture = setValue;    //���õ�����ʪ��ֵ
			displaySetValue();
		}
		if (strstr((char*)dataPtr, "led_pwm:") != NULL) {
			str1 = strstr((char*)dataPtr, "led_pwm:");

			while (*str1 < '0' || *str1 > '9')        //�ж��ǲ���0��9��Ч����
			{
				str1 = str1 + 1;
			}
			i = 0;
			while (*str1 >= '0' && *str1 <= '9')        //�ж��ǲ���0��9��Ч����
			{
				setvalue[i] = *str1;
				i++; str1++;
			}
			setValue = atoi(setvalue);
			if (led_flage) {
				ledduty = setValue;    //���õ�LED������ֵ
				LED_pwm_set(setValue);    //���õ�LED������ֵ
			}
			else {
				ledduty_t = setValue;    //���õ�LED������ֵ
			}
		}
		if (strstr((char*)dataPtr, "motor_pwm:") != NULL) {
			str1 = strstr((char*)dataPtr, "motor_pwm:");

			while (*str1 < '0' || *str1 > '9')        //�ж��ǲ���0��9��Ч����
			{
				str1 = str1 + 1;
			}
			i = 0;
			while (*str1 >= '0' && *str1 <= '9')        //�ж��ǲ���0��9��Ч����
			{
				setvalue[i] = *str1;
				i++; str1++;
			}
			setValue = atoi(setvalue);

			if (motor_flage) {
				motorduty = setValue;    //���õ�ˮ���ٶ�ֵ
				MOTOR_pwm_set(setValue);    //���õ�ˮ���ٶ�ֵ
			}
			else {
				motorduty_t = setValue;    //���õ�ˮ���ٶ�ֵ
			}
		}

	}
	if (sendFlag == 1)    //1�����ϴ�һ������
	{
		sendFlag = 0;

		memset(SEND_BUF, 0, sizeof(SEND_BUF));   			//��ջ�����
		sprintf(SEND_BUF, "{\"temp\":{\"value\":%d,\"set\":%d},\"humi\":%d,\"light\":{\"value\":%d,\"set\":%d},\"air\":{\"value\":%d,\"set\":%d},\"pwm\":{\"led\":%d,\"motor\":%d}}", (int)temperature, (int)setTempValue, (int)humidity, (int)light, (int)setLightValue, (int)soilMoisture, (int)setSoilMoisture, (int)ledduty, (int)motorduty);
		ESP8266_SendData((u8*)SEND_BUF, strlen(SEND_BUF));
	}
}
void Kalman_filter_init() {
	kalman_init(&light_kalman, (float)(Get_Adc_Average(ADC_Channel_9, 10) * 99.0), 5e2);
	kalman_init(&soilMoisture_kalman, (float)(Get_Adc_Average(ADC_Channel_8, 10) * 99.0), 5e2);
}
int main(void)
{
	delay_init();	           //��ʱ������ʼ��	 
	NVIC_Configuration();	   //�ж����ȼ�����
	I2C_Configuration();     //IIC��ʼ��
	OLED_Init();             //OLEDҺ����ʼ��
	Adc_Init();
	OLED_CLS();              //����
	KEY_GPIO_Init();        //�������ų�ʼ��    
	OLED_ShowStr(0, 2, "   loading...   ", 2, 0);//��ʾ������
	pwm_Init();
	ESP8266_Init();       //ESP8266��ʼ��
	MOTOR_GPIO_Init();
	Kalman_filter_init();
	while (DHT11_Init())
	{
		OLED_ShowStr(0, 2, "  DHT11 Error!  ", 2, 0);//��ʾDHT11����
		delay_ms(500);
	}
	OLED_CLS();              //����
	InitDisplay();
	TIM3_Init(99, 719);   //��ʱ����ʼ������ʱ1ms
	//Tout = ((arr+1)*(psc+1))/Tclk ; 
	//Tclk:��ʱ������Ƶ��(��λMHZ)
	//Tout:��ʱ�����ʱ��(��λus)
	while (1)
	{
		keyscan();  //����ɨ��

		if (setn == 0)     //��������״̬��
		{
			if (shuaxin == 1)        //���500msˢ��һ��
			{
				shuaxin = 0;
				displayDHT11TempAndHumi();
				displaySoilMoisture();
				displayLight();
				if (light <= setLightValue && led_flage) {
					ledduty_t = ledduty;
					ledduty = 99;
					led_flage = 0;
				}
				else if (!led_flage)
				{
					ledduty = ledduty_t;
					ledduty_t = 0;
					led_flage = 1;
				}

				LED_pwm_set(ledduty);
				if ((soilMoisture >= setSoilMoisture || temperature >= setTempValue) && motor_flage) {
					motorduty_t = motorduty;
					motorduty = 99;
					motor_flage = 0;
				}
				else if (!led_flage)
				{
					motorduty = motorduty_t;
					motorduty_t = 0;
					motor_flage = 1;
				}
				MOTOR_pwm_set(motorduty);
				if (light <= setLightValue || soilMoisture >= setSoilMoisture || temperature >= setTempValue)BEEP = 1; else BEEP = 0;
			}
		}
		UsartSendReceiveData();   //���ڷ��ͽ�������
		delay_ms(20);
	}
}

void TIM3_IRQHandler(void)//��ʱ��3�жϷ���������ڼ�¼ʱ��
{
	static u16 timeCount1 = 0;
	static u16 timeCount2 = 0;

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //����жϱ�־λ  

		timeCount1++;
		timeCount2++;
		if (timeCount1 >= 300)  //300ms
		{
			timeCount1 = 0;
			shanshuo = !shanshuo;
			shuaxin = 1;
		}
		if (timeCount2 >= 800)  //800ms
		{
			timeCount2 = 0;
			sendFlag = 1;
		}
	}
}

