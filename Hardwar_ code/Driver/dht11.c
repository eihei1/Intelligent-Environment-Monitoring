#include "dht11.h"
#include "delay.h"  

/*******************************************************************************
��������DHT11_IO_OUT
���ܣ�����IO���״̬
���룺
�����
����ֵ��
*******************************************************************************/
void DHT11_IO_OUT(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure); 
}

/*******************************************************************************
��������DHT11_IO_IN
���ܣ�����IO����״̬
���룺
�����
����ֵ��
*******************************************************************************/
void DHT11_IO_IN(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure); 
}

/*******************************************************************************
�������DDHT11_Init
���ܣ���ʼ��DHT11
���룺
�����
����ֵ����ʼ���ɹ�Ϊ0�����ɹ�Ϊ1
��ע��
*******************************************************************************/
u8 DHT11_Init(void)
{
	  unsigned char wait=0;
	
	  DHT11_IO_OUT();   //���ģʽ
	  DHT11_OUT_0;      //����
	  delay_ms(20);    //��������18ms
	  DHT11_OUT_1;     //����
	  delay_us(50);    //����20-40us
	  DHT11_IO_IN();   //����ģʽ
	  if(!READ_DHT11_IO)//�жϵ͵�ƽ�Ƿ�����
		{
				while(!READ_DHT11_IO && wait++<80){delay_us(1);}//�ȴ�40-50us�ĵ͵�ƽ����
				
				if(wait >= 80)return 1;
				else wait = 0;
					
				while(READ_DHT11_IO && wait++<80){delay_us(1);}//�ȴ�40-50us�ĸߵ�ƽ����
				
				if(wait >= 80)return 1;
				else return 0;
		}
		else
		{
				return 1;  //û�з�����Ӧ�źţ�ֱ�ӷ���1
		}
}

/*******************************************************************************
��������DHT11_ReadByte
���ܣ���DHT11��һ���ֽ�
���룺
�����
����ֵ����ȡ�����ֽ�
��ע��
*******************************************************************************/
unsigned char DHT11_ReadByte(void)
{
		unsigned char i;
	  unsigned char wait= 0;
	  unsigned char bit = 0;
    unsigned char dat = 0;
	
	  for (i=0; i<8; i++)//ѭ��8�Σ���һ���ֽ�
		{
			  wait = 0;
				while(!READ_DHT11_IO && wait++<80){delay_us(1);}//�ȴ�12-14us�ĵ͵�ƽ����
				wait = 0;
				delay_us(40);  //��ʱ40us
				bit = 0;       //����0
				if(READ_DHT11_IO)bit = 1;       //����1
        while(READ_DHT11_IO && wait++<150){delay_us(1);}//�ȴ�116-118us�ĸߵ�ƽ����
				if(wait >= 150)break;   //�ߵ�ƽ��ʱ
				dat <<= 1;   
				dat |= bit;  
		}
		return dat;
}

/*******************************************************************************
��������DHT11_Read_Data
���ܣ���DHT11��ȡ��ʪ��
���룺
�����
����ֵ��0��ȡ�ɹ���1��ȡʧ��
��ע��
*******************************************************************************/
u8 DHT11_Read_Data(u8 *Tdata,u8 *RHdata)
{
	  unsigned char i;
	  unsigned char BUF[5];
	
		if(DHT11_Init()==0)
		{
				for (i=0; i<5; i++)//ѭ��5�Σ���5���ֽ�
				{
						BUF[i] = DHT11_ReadByte();
				}
				if((BUF[0]+BUF[1]+BUF[2]+BUF[3])==BUF[4])//У��
				{
					*RHdata=BUF[1];//ʪ��ֵ
					*Tdata =BUF[2];//�¶�ֵ
				}
		}
		else
		{
				return 1;	//��ȡʧ��
		}
		return 0;	   //��ȡ�ɹ�
}
