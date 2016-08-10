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
void SJ485_Init( TASK task_id )
{
  /* Register task ID */
  SJ485_TaskID = task_id;   
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
EVENT SJ485_ProcessEvent( TASK task_id, EVENT events )
{
  osal_msg_hdr_t *msgPtr;
  
  (void)task_id;  // Intentionally unreferenced parameter

  
  /*????*/
  if ( events & SYS_EVENT_MSG )
  {
     msgPtr = osal_msg_receive(SJ485_TaskID);
 
     while (msgPtr) 
     {
       /* Do something here - for now, just deallocate the msg and move on */
       //SJ485_ProcessOSALMsg( msgPtr );
 	  
       /* De-allocate */
       osal_msg_deallocate( msgPtr );
       /* Next */
       msgPtr = osal_msg_receive( SJ485_TaskID );
     }
     return events ^ SYS_EVENT_MSG;
  }

	if ( events & SJ485_EVENT )
	{
			SJ485_make_msg();
			return events ^ SJ485_EVENT;
	}
  /* Nothing interested, discard the message */
  return 0;

}

/*********************************************************************
 * @fn      SJ485_ProcessOSALMsg()
 *
 * @brief   Process the incoming task message.
 *
 * @param   msgPtr - message to process
 *
 * @return  none
 
void SJ485_ProcessOSALMsg( osal_msg_hdr_t *msgPtr )
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
*/ 
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
NetErrorStatus SJ485_make_msg( void )
{
	u16 len;
  OsalSJ485Data_t* pkt_msg;

  pkt_msg = (OsalSJ485Data_t*)osal_msg_allocate( sizeof(OsalSJ485Data_t) );
  if( pkt_msg == NULL ) {
     return MEM_FULL;
  }  
  
	pkt_msg->msg = (u8*)pkt_msg + sizeof(OsalSJ485Data_t);
	RS485_Receive_Data(pkt_msg->msg,&len);
	pkt_msg->length = len;

  if( SUCCESS != osal_msg_send( App_TaskID, APP_SJ485_MSG, (osal_msg_hdr_t *)pkt_msg, 0 ) )
  {
    return INVALID_PARAM;
  }
  return SUCCESS;
}
/**************************************************************************************************
**************************************************************************************************/

