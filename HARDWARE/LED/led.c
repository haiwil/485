#include "led.h"

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA,PD端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LED0-->PB.6 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
 //GPIO_SetBits(GPIOB,GPIO_Pin_6);						 //PB.6 输出高
 GPIO_ResetBits(GPIOB,GPIO_Pin_6);
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	    		 //LED1-->PB.7 端口配置, 推挽输出
 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 //GPIO_SetBits(GPIOB,GPIO_Pin_7); 						 //PB.7 输出高 
 GPIO_ResetBits(GPIOB,GPIO_Pin_7);	
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	    		 //LED1-->PB.8 端口配置, 推挽输出
 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 //GPIO_SetBits(GPIOB,GPIO_Pin_8); 						 //PB.8 输出高 
 GPIO_ResetBits(GPIOB,GPIO_Pin_8);

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	    		 //LED1-->PB.9 端口配置, 推挽输出
 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 //GPIO_SetBits(GPIOB,GPIO_Pin_9); 						 //PB.9 输出高 	
 GPIO_ResetBits(GPIOB,GPIO_Pin_9);	
}
 
