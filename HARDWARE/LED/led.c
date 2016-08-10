#include "led.h"

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PA,PD�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LED0-->PB.6 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
 //GPIO_SetBits(GPIOB,GPIO_Pin_6);						 //PB.6 �����
 GPIO_ResetBits(GPIOB,GPIO_Pin_6);
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	    		 //LED1-->PB.7 �˿�����, �������
 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 //GPIO_SetBits(GPIOB,GPIO_Pin_7); 						 //PB.7 ����� 
 GPIO_ResetBits(GPIOB,GPIO_Pin_7);	
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	    		 //LED1-->PB.8 �˿�����, �������
 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 //GPIO_SetBits(GPIOB,GPIO_Pin_8); 						 //PB.8 ����� 
 GPIO_ResetBits(GPIOB,GPIO_Pin_8);

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	    		 //LED1-->PB.9 �˿�����, �������
 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 //GPIO_SetBits(GPIOB,GPIO_Pin_9); 						 //PB.9 ����� 	
 GPIO_ResetBits(GPIOB,GPIO_Pin_9);	
}
 
