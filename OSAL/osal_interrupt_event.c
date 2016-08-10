/**************************************************************************************************
  Filename:       osal_interrupt_event.h
  Revised:        $Date: 2016-01-11 $
  Revision:       $Revision: 00001 $

  Description:    This file contains the interface to the Drivers service.

**************************************************************************************************/



/**************************************************************************************************
 *                                            INCLUDES
 **************************************************************************************************/
#include "main.h"


vu32  interruptEvent = 0;

void osal_SetInterruptEvent( inter_event_t int_event_flag )
{
    //INTX_DISABLE();
  	interruptEvent |= int_event_flag;  // Stuff the event bit(s)
  	//INTX_ENABLE();
}

void osal_ClearInterruptEvent( inter_event_t int_event_flag )
{
    //INTX_DISABLE();
    interruptEvent &= ~(int_event_flag);  // Clear the event bit(s) 
    //INTX_ENABLE();
}

u32 InterruptEventProcess( void )
{

  if ( interruptEvent == 0 )  return 1;// no events
  
	
  if ( interruptEvent & USART_RX_FINISH_INTERRUPT_EVENT )
  {
     osal_ClearInterruptEvent( USART_RX_FINISH_INTERRUPT_EVENT );
     Usart_SendRxMsg();
  }
	/*
  if ( interruptEvent & KEY0_INTERRUPT_EVENT )
  {
     osal_ClearInterruptEvent( KEY0_INTERRUPT_EVENT );
     osal_update_timerEx( 10, Hal_TaskID, HAL_KEY_EVENT );
  }*/
    
	if ( interruptEvent & SJ485_INTERRUPT_EVENT )
  {
     osal_ClearInterruptEvent( SJ485_INTERRUPT_EVENT );
     osal_set_event( SJ485_TaskID, SJ485_EVENT );
  }
	
	if ( interruptEvent & SIM900A_INTERRUPT_EVENT )
  {
     osal_ClearInterruptEvent( SIM900A_INTERRUPT_EVENT );
     osal_set_event( SIM900A_TaskID, SIM900A_EVENT );
  }
  return 0;
}



