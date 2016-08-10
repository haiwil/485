/**************************************************************************************************
  Filename:       hal.h
  Revised:        $Date: 2016-1-8  $
  Revision:       $Revision: 00001 $

  Description:    This file contains the hal stack definitions.

**************************************************************************************************/
#ifndef __SJ485_H
#define __SJ485_H


/*********************************************************************
 * INCLUDES
 */
#include "types.h"
#include "485driver.h"
/*********************************************************************
 * CONSTANTS
 */



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

void SJ485_Init( TASK task_id );

EVENT SJ485_ProcessEvent( TASK task_id, EVENT events );
//void SJ485_ProcessOSALMsg( osal_msg_hdr_t *msgPtr );
NetErrorStatus SJ485_make_msg( void );

#endif


