#ifndef __USART_H
#define __USART_H

#include "stdio.h"	
#include "stdlib.h" 
#include "string.h" 
#include "sys.h" 
#include "types.h" 

#define Uart_Debug
#ifdef Uart_Debug
extern u8 debug_on;
#define Debug_printf  if(debug_on==TRUE)printf
#else
#define Debug_printf  /\
/Debug_printf
#endif

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
extern void uart_init(u32 bound);
extern NetErrorStatus Usart_SendRxMsg( void );

extern u8   USART1_Send_String(char* string);
extern void USART1_Send_Data(u8 *buf, u8 len);

#ifdef Uart_Debug
extern s32 msg_debug_cnt;
extern s32 stored_pkt_cnt;
extern s32 broadcast_cnt;
extern s32 resend_pkt_cnt;
void Debug_DealSerialCMD(OsalUSARTData_t *cmdMsg);
#endif

#endif


