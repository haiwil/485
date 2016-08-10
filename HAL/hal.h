/**************************************************************************************************
  Filename:       hal.h
  Revised:        $Date: 2016-1-8  $
  Revision:       $Revision: 00001 $

  Description:    This file contains the hal stack definitions.

**************************************************************************************************/
#ifndef __HAL_H
#define __HAL_H


/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

#define HAL_LED_BLINK_TIME                1000
#define RF_MAINTENANCE_PERIOD             50

/*********************************************************************
 * TYPEDEFS
 */
 	


/*********************************************************************
 * EXTERNAL VARIABLES
 */


/*********************************************************************
 * MACROS
 */



/*********************************************************************
 * FUNCTIONS
 */

void Hal_Init( TASK task_id );

EVENT Hal_ProcessEvent( TASK task_id, EVENT events );
void Hal_ProcessOSALMsg( osal_msg_hdr_t *msgPtr );
NetErrorStatus KEY_make_msg( u8 key_val );

#endif


