#ifndef _485DRIVER_H
#define _485DRIVER_H

#include "sys.h"  

#define USART2_MAX_RECV_LEN		128				//最大接收缓存字节数


extern u8 RS485_RX_BUF[USART2_MAX_RECV_LEN];
extern u8 RS485_RX_CNT;

extern void RS485_Init(u32 bound);				//串口2初始化 
extern void RS485_Send_Data(u8 *buf, u8 len);
extern u8 RS485_Send_String(char* string);
extern void RS485_Receive_Data(u8 *buf, u16 *len);
extern void Reset_485(void);
#endif
