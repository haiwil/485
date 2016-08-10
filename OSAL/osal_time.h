#ifndef __OSAL_TIME_H
#define __OSAL_TIME_H

#include "main.h"

#define MAX_WAKEUP_PERIOD       0xFFFF


#define TIMER_SLOT             200
#define RATIO_OF_TIMESTAMP     (1000/TIMER_SLOT)



typedef struct _NODE
{  
  TIMESTAMP 	timestamp;//事件延后执行的时间点
  TASK 			task;
  EVENT 		event;
  struct _NODE* timer_next;
}osal_timer_t;


typedef EVENT (*pTaskEventHandlerFn)( TASK task_id, EVENT event );
extern const pTaskEventHandlerFn tasksArr[];
extern const uint8 tasksCnt;
extern EVENT  taskEvent[];
extern osal_timer_t *timer_head;

#include "osal_time.h"

//(内部调用) 
osal_timer_t* add_node( TIMESTAMP timestamp_value, TASK task_value, EVENT event_value, u8 update);
OsalErrorStatus delete_node( TASK task_value, EVENT event_value );
osal_timer_t* insert_node(uint8 timestamp_value, TASK task_value, EVENT event_value, uint8 place);

/*......................................................................*/
//(外部调用) 
osal_timer_t* osalFindTimer( TASK task_id, EVENT event_flag );
void    TimeoutEventSetPoll(void);
state_t osal_update_timerEx( TIMESTAMP timestamp_value, TASK task_value, EVENT event_value );
state_t osal_add_timerEx( TIMESTAMP timestamp_value, TASK task_value, EVENT event_value );
state_t osal_update_128us_timerEx( TIMESTAMP timestamp_in_128us, TASK task_value, EVENT event_value );
state_t osal_delete_timerEx( TASK task_value, EVENT event_value );
state_t osal_set_event( TASK taskid, EVENT event_flag );
state_t osal_clear_event( uint8 task_id, uint16 event_flag );

#endif
