#ifndef OSAL_H
#define OSAL_H

#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */


/*********************************************************************
 * MACROS
 */
#define DISABLE_INTERRUPT()     //__bic_SR_register(GIE)
#define ENABLE_INTERRUPT()      //__bis_SR_register(GIE)

#define HAL_ENTER_CRITICAL_SECTION(x)   //st( x = __get_SR_register(); DISABLE_INTERRUPT(); )
#define HAL_EXIT_CRITICAL_SECTION(x)    //st( __bis_SR_register(x); )

#define HAL_ENTER_LOW_POWER_MODE()      //LPM3     



#if ( UINT_MAX == 65535 ) /* 8-bit and 16-bit devices */
  #define osal_offsetof(type, member) ((uint16) &(((type *) 0)->member))
#else /* 32-bit devices */
  #define osal_offsetof(type, member) ((uint32) &(((type *) 0)->member))
#endif

#define OSAL_MSG_NEXT(msg_ptr)      ((osal_msg_hdr_t *)msg_ptr)->next

#define OSAL_MSG_Q_INIT(q_ptr)      *(q_ptr) = NULL

#define OSAL_MSG_Q_EMPTY(q_ptr)     (*(q_ptr) == NULL)

#define OSAL_MSG_Q_HEAD(q_ptr)      ((osal_msg_hdr_t *)q_ptr + 1)

#define OSAL_MSG_LEN(msg_ptr)      ((osal_msg_hdr_t *)msg_ptr)->len

#define OSAL_MSG_ID(msg_ptr)      ((osal_msg_hdr_t *)msg_ptr)->dest_id

#define OSAL_MSG_EVT(msg_ptr)      ((osal_msg_hdr_t *)msg_ptr)->event

#define OSAL_MSG_TIMESTAMP(msg_ptr)      ((osal_msg_hdr_t *)msg_ptr)->timestamp

#define MsgMInit   LinkedListMemInit
#define MsgMalloc  LinkedListMalloc
#define MsgFree    LinkedListMemFree

/*********************************************************************
 * CONSTANTS
 */

/*** NULL Task ***/
#define TASK_NO_TASK      0xFF

/*********************************************************************
 * TYPEDEFS
 */

typedef unsigned short halIntState_t;

/*
typedef struct {
// The 1 LSB of 'val' is used as a boolean to indicate need ack or not.
unsigned ack_required : 1;
// The 1 LSB of 'val' is used as a boolean to indicate LinkQuality or rssi.
unsigned RSSI_transformat_required : 1;
// The 6 MSB's of 'val' indicate the type of command, in 8-bit bytes.
unsigned cmd_type : 6;
} osal_msg_cmd_t;
*/

/*************  KEY massage format  *******************/



/*************  UART massage format  *******************/



/*********************************************************************
 * GLOBAL VARIABLES
 */
extern osal_msg_q_t osal_qHead;

/*********************************************************************
 * FUNCTIONS
 */

/*** Message Management ***/

/*
 * Message Head Initiaion
 */
extern void osal_msg_unit( void );

  /*
   * Task Message Allocation
   */
  extern osal_msg_hdr_t * osal_msg_allocate( uint16 len );

  /*
   * Task Message Deallocation
   */
  extern state_t osal_msg_deallocate( osal_msg_hdr_t *msg_ptr );

  /*
   * Send a Task Message
   */
  extern state_t osal_msg_send( TASK destination_task, EVENT dest_event, osal_msg_hdr_t *msg_ptr, TIMESTAMP timeout );

  /*
   * Receive a Task Message
   */
  extern osal_msg_hdr_t *osal_msg_receive( TASK task_id );

  /*
   * Find in place a matching Task Message / Event.
   */
  extern osal_msg_hdr_t *osal_msg_find(TASK task_id, EVENT event);

  /*
   * Enqueue a Task Message
   */
  extern void osal_msg_enqueue( osal_msg_q_t *q_ptr, void *msg_ptr );

  /*
   * Enqueue a Task Message Up to Max
   */
  extern state_t osal_msg_enqueue_max( osal_msg_q_t *q_ptr, void *msg_ptr, uint8 max );

  /*
   * Dequeue a Task Message
   */
  extern void *osal_msg_dequeue( osal_msg_q_t *q_ptr );

  /*
   * Push a Task Message to head of queue
   */
  extern void osal_msg_push( osal_msg_q_t *q_ptr, void *msg_ptr );

  /*
   * Extract and remove a Task Message from queue
   */
  extern void osal_msg_extract( osal_msg_q_t *q_ptr, void *msg_ptr, void *prev_ptr );



/*********************************************************************
*********************************************************************/

  
#ifdef __cplusplus
}
#endif

#endif /* OSAL_H */
