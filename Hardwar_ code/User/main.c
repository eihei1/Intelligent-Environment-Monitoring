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
unsigned char setn = 0;//记录设置键按下的次数
unsigned char temperature = 0;
unsigned char humidity = 0; //湿度
unsigned char setTempValue = 35;        //温度设置值
unsigned int  light = 0; //光照强度
unsigned int  setSoilMoisture = 50;       //土壤湿度设置值
unsigned int  soilMoisture;           //土壤湿度
unsigned int setLightValue = 20;       //光照设置值
unsigned int ledduty = 0, motorduty = 0;
unsigned int ledduty_t = 0, motorduty_t = 0;
bool led_flage = 1, motor_flage = 1;//1正常 0警告
kalman light_kalman;
kalman soilMoisture_kalman;
int8_t n = 0;
bool shuaxin = 0;
bool shanshuo = 0;
bool sendFlag = 1;

void InitDisplay(void)   //初始化显示
{
	unsigned char i = 0;
	for (i = 0;i < 4;i++)OLED_ShowCN(i * 16, 0, i + 0, 0);//显示中文：环境温度
	for (i = 0;i < 4;i++)OLED_ShowCN(i * 16, 2, i + 4, 0);//显示中文：环境湿度
	for (i = 0;i < 4;i++)OLED_ShowCN(i * 16, 4, i + 8, 0);//显示中文：土壤湿度
	for (i = 0;i < 4;i++)OLED_ShowCN(i * 16, 6, i + 12, 0);//显示中文：光照强度
	OLED_ShowChar(64, 0, ':', 2, 0);
	OLED_ShowChar(64, 2, ':', 2, 0);
	OLED_ShowChar(64, 4, ':', 2, 0);
	OLED_ShowChar(64, 6, ':', 2, 0);
}

void displayDHT11TempAndHumi(void)  //显示环境温湿度
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

void displayLight(void)//显示光强度
{
	u16 test_adc = 0;
	/////////////获取光线值
	test_adc = Get_Adc_Average(ADC_Channel_9, 10);//读取通道9的10次AD平均值
	light = (unsigned int)kalman_filter(&light_kalman, (float)(test_adc * 99.0)) / 4096;//转换成0-99百分比
	light = light >= 99 ? 99 : light;//最大只能到百分之99
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

void displaySoilMoisture(void)//显示土壤湿度
{
	soilMoisture = (unsigned int)(kalman_filter(&soilMoisture_kalman, (float)(Get_Adc_Average(ADC_Channel_8, 10) * 99.0)) / 4096);//转换成0-99百分比
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

void displaySetValue(void)  //显示设置的值
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

void keyscan(void)   //按键扫描
{
	u8 i;

	if (KEY1 == 0) //设置
	{
		delay_ms(20);
		if (KEY1 == 0)
		{
			while (KEY1 == 0);
			BEEP = 0;
			setn++;
			if (setn == 1)
			{
				OLED_CLS();    //清屏
				for (i = 0;i < 2;i++)
				{
					OLED_ShowCN(i * 16 + 16, 0, i + 16, 0);  //显示设置
				}
				for (i = 0;i < 4;i++)OLED_ShowCN(i * 16 + 48, 0, i + 8, 0);//显示中文：土壤湿度
			}
			if (setn == 2)
			{
				for (i = 0;i < 4;i++)OLED_ShowCN(i * 16 + 48, 0, i + 0, 0);//显示中文：环境温度
			}
			if (setn == 3)
			{
				for (i = 0;i < 4;i++)OLED_ShowCN(i * 16 + 48, 0, i + 12, 0);//显示中文：光照强度
				OLED_ShowChar(84, 4, ' ', 2, 0);
			}
			displaySetValue();
			if (setn > 3)
			{
				setn = 0;
				OLED_CLS();    //清屏
				InitDisplay();
			}
		}
	}
	if (KEY2 == 0) //加
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
	if (KEY3 == 0) //减
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

	dataPtr = ESP8266_GetIPD(0);   //接收数据
	if (dataPtr != NULL)
	{
		if (strstr((char*)dataPtr, "light:") != NULL)
		{
			BEEP = 1;
			delay_ms(80);
			BEEP = 0;

			str1 = strstr((char*)dataPtr, "light:");

			while (*str1 < '0' || *str1 > '9')    //判断是不是0到9有效数字
			{
				str1 = str1 + 1;
			}
			i = 0;
			while (*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
			{
				setvalue[i] = *str1;
				i++; str1++;
			}
			setValue = atoi(setvalue);
			setLightValue = setValue;    //设置的光照值
			displaySetValue();
		}

		if (strstr((char*)dataPtr, "temp:") != NULL)
		{
			str1 = strstr((char*)dataPtr, "temp:");

			while (*str1 < '0' || *str1 > '9')        //判断是不是0到9有效数字
			{
				str1 = str1 + 1;
			}
			i = 0;
			while (*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
			{
				setvalue[i] = *str1;
				i++; str1++;
			}
			setValue = atoi(setvalue);
			setTempValue = setValue;    //设置的温度值
			displaySetValue();
		}

		if (strstr((char*)dataPtr, "soil_hmd:") != NULL)
		{
			str1 = strstr((char*)dataPtr, "soil_hmd:");

			while (*str1 < '0' || *str1 > '9')        //判断是不是0到9有效数字
			{
				str1 = str1 + 1;
			}
			i = 0;
			while (*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
			{
				setvalue[i] = *str1;
				i++; str1++;
			}
			setValue = atoi(setvalue);
			setSoilMoisture = setValue;    //设置的土壤湿度值
			displaySetValue();
		}
		if (strstr((char*)dataPtr, "led_pwm:") != NULL) {
			str1 = strstr((char*)dataPtr, "led_pwm:");

			while (*str1 < '0' || *str1 > '9')        //判断是不是0到9有效数字
			{
				str1 = str1 + 1;
			}
			i = 0;
			while (*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
			{
				setvalue[i] = *str1;
				i++; str1++;
			}
			setValue = atoi(setvalue);
			if (led_flage) {
				ledduty = setValue;    //设置的LED灯亮度值
				LED_pwm_set(setValue);    //设置的LED灯亮度值
			}
			else {
				ledduty_t = setValue;    //设置的LED灯亮度值
			}
		}
		if (strstr((char*)dataPtr, "motor_pwm:") != NULL) {
			str1 = strstr((char*)dataPtr, "motor_pwm:");

			while (*str1 < '0' || *str1 > '9')        //判断是不是0到9有效数字
			{
				str1 = str1 + 1;
			}
			i = 0;
			while (*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
			{
				setvalue[i] = *str1;
				i++; str1++;
			}
			setValue = atoi(setvalue);

			if (motor_flage) {
				motorduty = setValue;    //设置的水泵速度值
				MOTOR_pwm_set(setValue);    //设置的水泵速度值
			}
			else {
				motorduty_t = setValue;    //设置的水泵速度值
			}
		}

	}
	if (sendFlag == 1)    //1秒钟上传一次数据
	{
		sendFlag = 0;

		memset(SEND_BUF, 0, sizeof(SEND_BUF));   			//清空缓冲区
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
	delay_init();	           //延时函数初始化	 
	NVIC_Configuration();	   //中断优先级配置
	I2C_Configuration();     //IIC初始化
	OLED_Init();             //OLED液晶初始化
	Adc_Init();
	OLED_CLS();              //清屏
	KEY_GPIO_Init();        //按键引脚初始化    
	OLED_ShowStr(0, 2, "   loading...   ", 2, 0);//显示加载中
	pwm_Init();
	ESP8266_Init();       //ESP8266初始化
	MOTOR_GPIO_Init();
	Kalman_filter_init();
	while (DHT11_Init())
	{
		OLED_ShowStr(0, 2, "  DHT11 Error!  ", 2, 0);//显示DHT11错误！
		delay_ms(500);
	}
	OLED_CLS();              //清屏
	InitDisplay();
	TIM3_Init(99, 719);   //定时器初始化，定时1ms
	//Tout = ((arr+1)*(psc+1))/Tclk ; 
	//Tclk:定时器输入频率(单位MHZ)
	//Tout:定时器溢出时间(单位us)
	while (1)
	{
		keyscan();  //按键扫描

		if (setn == 0)     //不在设置状态下
		{
			if (shuaxin == 1)        //大概500ms刷新一下
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
		UsartSendReceiveData();   //串口发送接收数据
		delay_ms(20);
	}
}

void TIM3_IRQHandler(void)//定时器3中断服务程序，用于记录时间
{
	static u16 timeCount1 = 0;
	static u16 timeCount2 = 0;

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //清除中断标志位  

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

