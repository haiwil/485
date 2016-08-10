
#ifndef __OSAL_H
#define __OSAL_H


#include "main.h"
/*********************************************************************
 * TYPEDEFS
 */

void osalInitTasks( void );
void osal_init_driver(void);
state_t osal_init_system(void);
void osal_run_system(void);
void osal_Poll_Event(void);
u8 osal_Check_Event(void);
void osal_run_system(void);
extern uint8 osal_memcmp( const void *src1, const void *src2, unsigned int len );
 /*周期型任务*/
void osal_Poll_Periodic_Task(void);

#endif

