/**************************************************************************************************
  Filename:       osal_interrupt_event.h
  Revised:        $Date: 2016-01-11 $
  Revision:       $Revision: 00001 $

  Description:    This file contains the interface to the Drivers service.

**************************************************************************************************/

#ifndef OSAL_INTERRUPT_EVENT_H
#define OSAL_INTERRUPT_EVENT_H
#include "types.h"
/*  INTERRUPT EVENT  MASK */
#define RTC_SEC_INTERRUPT_EVENT          ((u32)0x00000001)  
#define USART_RX_FINISH_INTERRUPT_EVENT ((u32)0x00000002) 
#define KEY0_INTERRUPT_EVENT            ((u32)0x00000004) 
#define SJ485_INTERRUPT_EVENT            ((u32)0x00000008)
#define SIM900A_INTERRUPT_EVENT            ((u32)0x00000010)


void osal_SetInterruptEvent( inter_event_t int_event_flag );

void osal_ClearInterruptEvent( inter_event_t int_event_flag );
extern u32 InterruptEventProcess( void );

#endif
