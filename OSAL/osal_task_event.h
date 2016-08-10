/**************************************************************************************************
  Filename:       osal_task_event.h
  Revised:        $Date: 2015-12-02 $
  Revision:       $Revision: 00001 $

  Description:    This file contains the interface to the Drivers service.

**************************************************************************************************/

#ifndef OSAL_TASK_EVENT_H
#define OSAL_TASK_EVENT_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include "types.h"

/**************************************************************************************************
 * MACROS
 **************************************************************************************************/



/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/
  
/*********************************************************************
* Global System Events
*/

#define SYS_EVENT_MSG         0x8000  // A message is waiting event
  
  
/*   MAC EVENTS    */
#define MAC_NIRQ_EVENT          	   BV(0)

/*   MAC MSG EVENTS    */
#define MAC_RF_INIT_MSG         BV(0)


/*   HAL EVENTS    */
#define HAL_KEY_EVENT                    BV(0)
#define HAL_LED_BLINK_EVENT              BV(1)
#define HAL_SLEEP_TIMER_EVENT            BV(2)

/*   HAL MSG EVENTS    */
#define HAL_SERIAL_MSG        BV(0)

/*   SJ485 MSG EVENTS    */
#define SJ485_EVENT					  BV(0)

/*   SIM900A MSG EVENTS    */
#define SIM900A_EVENT		      			  			BV(0)
#define SIM900A_CONNECT_GPRS_EVENT    			BV(1)
#define SIM900A_SET_AT_TIME_OUT_EVENT 			BV(2)
#define SIM900A_PREPARE_SEND_DATA_EVENT  		BV(3)
#define SIM900A_SEND_DATA_EVENT          		BV(4)
#define SIM900A_SEND_END_DATA_EVENT					BV(5)
#define SIM900A_CHECK_SEND_FIFO_EVENT				BV(6)

/*   APP EVENTS    */
#define APP_NETWORK_CONFIG_EVENT				    BV(1)
#define APP_REGISTER_EVENT          				BV(2)
#define APP_SIM900A_HEART_BEAT_EVENT        BV(3)
#define APP_RS485_LED_BLINK_EVENT   				BV(4)

/*   APP MSG EVENTS    */
#define APP_KEY_CHANGE_MSG          BV(0)
#define APP_SERIAL_MSG              BV(1)
#define APP_SJ485_MSG								BV(2)
#define	APP_SIM900A_MSG							BV(3)
/**************************************************************************************************
 * TYPEDEFS
 **************************************************************************************************/


/**************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/
extern TASK MAC_TaskID;   // Task ID for internal task/event processing
extern TASK Hal_TaskID;
extern TASK SJ485_TaskID;
extern TASK SIM900A_TaskID;
extern TASK App_TaskID;



/**************************************************************************************************
 * FUNCTIONS - API
 **************************************************************************************************/

/**************************************************************************************************
**************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
