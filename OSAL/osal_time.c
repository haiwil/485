#include "osal_time.h"

/*********************************************************************
 * LOCAL VARIABLES
 */
// Milliseconds since last reboot
//static uint32 osal_systemClock;

/*********************************************************************
 * EXTERNAL VARIABLES
 */
osal_timer_t *timer_head = NULL;
const pTaskEventHandlerFn tasksArr[] = {
  Hal_ProcessEvent,
	#if SIM_DEBUG
	SIM900A_ProcessEvent,
	#endif
	SJ485_ProcessEvent,
  App_ProcessEvent
};

const uint8 tasksCnt = sizeof( tasksArr ) / sizeof( tasksArr[0] );
EVENT taskEvent[sizeof( tasksArr ) / sizeof( tasksArr[0] )] = {0};


//timestamp_value:多久执行任务
//返回值:动态分配的osal_timer_t* 
osal_timer_t* add_node( TIMESTAMP timestamp_value, TASK task_value, EVENT event_value, u8 update)
{
  osal_timer_t* pNode;
  osal_timer_t* temp;

  if( task_value >= tasksCnt )
    return NULL;
  if(timer_head == NULL)
  {
    pNode = (osal_timer_t*) TimerEventListMalloc(sizeof(osal_timer_t));
    if( pNode == NULL )
      return NULL;
    timer_head = pNode;
    timer_head->timestamp = timestamp_value + e_systick_val;
    timer_head->task = task_value;
    timer_head->event = event_value;
    timer_head->timer_next = NULL;
  }
  else
  {
    temp = timer_head;
    if( update == TRUE )
    {
        while( (temp->timer_next != NULL) || (temp->task == task_value && temp->event == event_value) )
        {
          if (temp->task == task_value && temp->event == event_value)//已经存在链表中
          {
            temp->timestamp = timestamp_value + e_systick_val;
            return temp;
          }
          temp = temp->timer_next;
        }
    }
    else
    {
        while(temp->timer_next != NULL)
        {
          temp = temp->timer_next;
        }
    }
    pNode = (osal_timer_t*) TimerEventListMalloc(sizeof(osal_timer_t));
    if( pNode == NULL )
      return NULL;
    temp->timer_next = pNode;
    pNode->task = task_value;
    pNode->event = event_value;
    pNode->timestamp = timestamp_value + e_systick_val;
    pNode->timer_next = NULL;
  }
  return pNode;
}

//返回值:ErrorStatus  EMPTY_QUEUE  SUCCESS  INVALID_PARAM
OsalErrorStatus delete_node( TASK task_value, EVENT event_value )
{
  osal_timer_t* pNode_front;
  osal_timer_t* pNode_present;
  
  if(timer_head == NULL)
    return EMPTY_QUEUE;
  
  pNode_present = timer_head;
  pNode_front = pNode_present;
  while( (pNode_present != NULL) && 
      (!((pNode_present->task == task_value) && (pNode_present->event == event_value)))  )
  {
    pNode_front = pNode_present;
    pNode_present = pNode_present->timer_next;
  }
  if( pNode_present != NULL )
  {
    if(pNode_present == timer_head)
    {
      timer_head = timer_head->timer_next;
    }
    else
    {
      pNode_front->timer_next = pNode_present->timer_next;
    }
    TimerEventListMemFree(pNode_present);
    return SUCCESS;
  }
  return INVALID_PARAM;
}

#if 0
//timestamp_value:多久执行任务
//返回值:动态分配的osal_timer_t* 
osal_timer_t* insert_node(uint8 timestamp_value, TASK task_value, EVENT event_value, uint8 place)
{
  osal_timer_t* pNode_front;
  osal_timer_t* pNode_present;
  osal_timer_t* pInsert;
  uint16 i = 0;
  
  delete_node(task_value, event_value);
  
  if(timer_head == NULL)
  {
    return add_node(timestamp_value, task_value, event_value, FALSE);
  }
  
  pNode_present = timer_head;
  pNode_front = pNode_present;
  for(i = 0; i < place; i++)
  {
    pNode_front = pNode_present;
    pNode_present = pNode_present->timer_next;
    if(pNode_present == NULL)
    {
      break;
    }
  }
  pInsert = (osal_timer_t*) TimerEventListMalloc(sizeof(osal_timer_t));
  if( pInsert == NULL )
    return NULL;
  pInsert->timer_next = pNode_present;
  pInsert->timestamp = timestamp_value + e_systick_val;
  pInsert->task = task_value;
  pInsert->event = event_value;
  if(place != 0)
  {
    if( pNode_front == pNode_present )
      timer_head = pInsert;
    else
      pNode_front->timer_next = pInsert;
  }
  else
  {
    timer_head = pInsert;
  }
  return pInsert;
}
#endif

//返回值:任务事件的结构体指针
osal_timer_t* osalFindTimer( TASK task_id, EVENT event_flag )
{
  osal_timer_t* pNode;
  pNode = timer_head;
  while(pNode != NULL)
  {
    if(pNode->task == task_id && pNode->event == event_flag)
    {
      return pNode;
    }
    pNode = pNode->timer_next;
  }
  return NULL;
}

//置位超时事件标志位,并从链表删除对应的定时任务
void TimeoutEventSetPoll(void)
{
  osal_timer_t* pCNT = timer_head;
  osal_timer_t* prev = pCNT;
  osal_timer_t* pDelete = pCNT;
  while(pCNT != NULL)
  {
      if(time_after(e_systick_val,pCNT->timestamp))//执行任务事件的时间到了
  	  {
      	osal_set_event( pCNT->task, pCNT->event );
        if( pCNT == timer_head )
        {
          pDelete = pCNT;
          timer_head = timer_head->timer_next;  
        }
        else
        {
          pDelete = pCNT;
  		  prev->timer_next = pCNT->timer_next;
        }
        pCNT = pCNT->timer_next;
        TimerEventListMemFree(pDelete);
  	  }  
      else
      {
        prev = pCNT;
        pCNT = pCNT->timer_next;
      }     
  }
}

//把节点加到链表尾，不管链表是否存在事件
//timestamp_value:多久执行任务
//返回值:SUCCESS  MEM_FULL
state_t osal_add_timerEx( TIMESTAMP timestamp_value, TASK task_value, EVENT event_value )
{
  TIMESTAMP timestamp_in_128us = timestamp_value<<3;
  osal_timer_t* newTimer;
  newTimer = add_node( timestamp_in_128us, task_value, event_value,FALSE);
  return ( (newTimer != NULL) ? SUCCESS : MEM_FULL );
}

//更新指定事件中第一个事件的执行时间，事件不存在则添加事件
//timestamp_value:多久执行任务
//返回值:SUCCESS  MEM_FULL
state_t osal_update_timerEx( TIMESTAMP timestamp_value, TASK task_value, EVENT event_value )
{
  TIMESTAMP timestamp_in_128us = timestamp_value<<3;
  osal_timer_t* newTimer;
  newTimer = add_node( timestamp_in_128us, task_value, event_value,TRUE);
  return ( (newTimer != NULL) ? SUCCESS : MEM_FULL );
}

//更新指定事件中第一个事件的执行时间，事件不存在则添加事件
//timestamp_value:多久执行任务
//返回值:SUCCESS  MEM_FULL
state_t osal_update_128us_timerEx( TIMESTAMP timestamp_in_128us, TASK task_value, EVENT event_value )
{
  osal_timer_t* newTimer;
  newTimer = add_node( timestamp_in_128us, task_value, event_value,TRUE);
  return ( (newTimer != NULL) ? SUCCESS : MEM_FULL );
}

//返回值:ErrorStatus  EMPTY_QUEUE  SUCCESS  INVALID_PARAM
state_t osal_delete_timerEx( TASK task_value, EVENT event_value )
{
  state_t state;
//  halIntState_t intState;
//  HAL_ENTER_CRITICAL_SECTION(intState);  // Hold off interrupts.

  // delete timer
  state = delete_node( task_value, event_value );

  //HAL_EXIT_CRITICAL_SECTION(intState);  // Re-enable interrupts.
  return ( (state == SUCCESS) ? SUCCESS : ERROR );
}

//置位任务的某个事件
state_t osal_set_event( TASK task_id, EVENT event_flag )
{
  if ( task_id < tasksCnt )
  {
    taskEvent[task_id] |= event_flag;  // Stuff the event bit(s)
    return ( SUCCESS );
  }
  else
  {
    return INVALID_PARAM;
  }
}

//返回值:state_t    SUCCESS  INVALID_PARAM
state_t osal_clear_event( uint8 task_id, uint16 event_flag )
{
  if ( task_id < tasksCnt )
  {
    taskEvent[task_id] &= ~(event_flag);     // Clear the event bit(s) 
    return ( SUCCESS );
  }
  else
  {
    return INVALID_PARAM;
  }
}


