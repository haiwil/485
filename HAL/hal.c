/**************************************************************************************************
  Filename:       hal.c
  Revised:        $Date: 2016-1-8 $
  Revision:       $Revision: 00001 $

  Description:    This file contains the interface to the Drivers Service.
**************************************************************************************************/


/**************************************************************************************************
 *                                            INCLUDES
 **************************************************************************************************/
#include "main.h"

/**************************************************************************************************
 *                                            MACROS
 **************************************************************************************************/



/**************************************************************************************************
 *                                          CONSTANTS
 **************************************************************************************************/


/**************************************************************************************************
 *                                          TYPEDEFS
 **************************************************************************************************/


/**************************************************************************************************
 *                                      GLOBAL VARIABLES
 **************************************************************************************************/

 
/**************************************************************************************************
 *										FUNCTIONS PROTOTYPES
 **************************************************************************************************/


/**************************************************************************************************
 *                                      FUNCTIONS - API
 **************************************************************************************************/

/**************************************************************************************************
 * @fn      Hal_Init
 *
 * @brief   Hal Initialization function.
 *
 * @param   task_id - Hal TaskId
 *
 * @return  None
 **************************************************************************************************/
void Hal_Init( TASK task_id )
{
  /* Register task ID */
  Hal_TaskID = task_id;   
  osal_update_timerEx( HAL_LED_BLINK_TIME, Hal_TaskID, HAL_LED_BLINK_EVENT);
}

/**************************************************************************************************
 * @fn      Hal_ProcessEvent
 *
 * @brief   Hal Process Event
 *
 * @param   task_id - Hal TaskId
 *          events - events
 *
 * @return  None
 **************************************************************************************************/
EVENT Hal_ProcessEvent( TASK task_id, EVENT events )
{
  osal_msg_hdr_t *msgPtr;
  
  (void)task_id;  // Intentionally unreferenced parameter

  if (events & HAL_KEY_EVENT)
  {
    //KEY_make_msg( Get_KEY_Value() );     
    return events ^ HAL_KEY_EVENT;
  }
  
  /*消息事件*/
  if ( events & SYS_EVENT_MSG )
  {
     msgPtr = osal_msg_receive(Hal_TaskID);
 
     while (msgPtr) 
     {
       /* Do something here - for now, just deallocate the msg and move on */
       Hal_ProcessOSALMsg( msgPtr );
 	  
       /* De-allocate */
       osal_msg_deallocate( msgPtr );
       /* Next */
       msgPtr = osal_msg_receive( Hal_TaskID );
     }
     return events ^ SYS_EVENT_MSG;
  }

  if (events & HAL_LED_BLINK_EVENT)
  {  
    LED0=!LED0;
		//LED1=!LED1;
		//LED2=!LED2;
		//LED3=!LED3;
		//MY_IWDG_Feed();
		//USART1_Send_String("led toggle \n");
  	osal_update_timerEx( HAL_LED_BLINK_TIME, Hal_TaskID, HAL_LED_BLINK_EVENT);
    return events ^ HAL_LED_BLINK_EVENT;
  }

  /* Nothing interested, discard the message */
  return 0;

}

/*********************************************************************
 * @fn      Hal_ProcessOSALMsg()
 *
 * @brief   Process the incoming task message.
 *
 * @param   msgPtr - message to process
 *
 * @return  none
 */
void Hal_ProcessOSALMsg( osal_msg_hdr_t *msgPtr )
{
  switch ( OSAL_MSG_EVT(msgPtr) )
  {
    case HAL_SERIAL_MSG:	   
			//USART_SendMsg( OSAL_USARTDATA_MSGp(msgPtr), OSAL_USARTDATA_LEN(msgPtr) );
      break;
      
    default:
      break;
  }
}

/**************************************************************************************************
 *                                      LOCAL_FUNCTIONS - API
 **************************************************************************************************/
	


/**************************************************************************************************
* @fn		KEY_make_msg
*
* @brief	get the KEY value, and package into a message send to APP task
*
* @param	none
*
* @return	NetErrorStatus
**************************************************************************************************/
NetErrorStatus KEY_make_msg( u8 key_val )
{
  keyChange_t* pkt_msg;
  if(key_val==0) {
    return INVALID_PARAM;
  }
  pkt_msg = (keyChange_t*)osal_msg_allocate( sizeof(keyChange_t) );
  if( pkt_msg == NULL ) {
     return MEM_FULL;
  }  
  pkt_msg->keys = key_val;
  if( SUCCESS != osal_msg_send( App_TaskID, APP_KEY_CHANGE_MSG, (osal_msg_hdr_t *)pkt_msg, 0 ) )
  {
    return INVALID_PARAM;
  }
  return SUCCESS;
}
/**************************************************************************************************
**************************************************************************************************/

