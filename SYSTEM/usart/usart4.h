#ifndef _USART4_H
#define _USART4_H

#include "sys.h"  

#define USART4_MAX_RECV_LEN		512				//最大接收缓存字节数


extern u8 USART4_RX_BUF[USART4_MAX_RECV_LEN];
extern u8 USART4_RX_CNT;

extern void UART4_Init(u32 bound);				//串口2初始化 
extern void UART4_Send_Data(u8 *buf, u8 len);
extern u8 UART4_Send_String(char* string);
extern void UART4_Receive_Data(u8 *buf, u8 *len);
extern void UART4_RxDataProcess(void);
#endif
