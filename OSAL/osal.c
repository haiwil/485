
#include "main.h"


void osal_init_driver(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init();  		//初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	UART4_Init(9600);
	RS485_Init(9600);
	LED_Init();					//初始化LED 
 	//KEY_Init();					//按键初始化 
}

state_t osal_init_system(void)
{
  // Initialize the Memory Allocation System
  LinkedListMemInit();
  TimerEventListMemInit();

  // Initialize the message queue
  osal_msg_unit();
  // Initialize the system tasks.
  osalInitTasks();
	
  return ( SUCCESS );
}

void osalInitTasks( void )
{
  uint8 taskID = 0;                  
  Hal_Init( taskID++ );              //modified if hardware changes 
	#if SIM_DEBUG
	SIM900A_Init( taskID++ );
	#endif
	SJ485_Init( taskID++ );	
  App_Init( taskID );                //mostly modified part
   
}

//执行每个任务中的各个事件
void osal_Poll_Event(void)
{
  uint8 idx = 0;
  do {
    if (taskEvent[idx])  
    {
      break;         
    }
  } while (++idx < tasksCnt);

  if (idx < tasksCnt)
  {
    EVENT events;
    events = taskEvent[idx]; 
    taskEvent[idx] = 0; 
    events = (tasksArr[idx])( idx, events );
    taskEvent[idx] |= events; 
  }
}

 /*周期型任务*/
void osal_Poll_Periodic_Task(void)
{
  InterruptEventProcess( );
}

 
//查看是否有事件
//返回值:0 有事件未执行  1 没有事件可执行
u8 osal_Check_Event(void)
{
  uint8 idx = 0; 
  do
  {
    if(taskEvent[idx])
    {
      return 0;
    }
  }while(++idx < tasksCnt);
  return 1;
}

void osal_run_system(void)
{
  while(1)
  {
    TimeoutEventSetPoll();	    
    osal_Poll_Event();   
		osal_Poll_Periodic_Task();
  }
}

/*********************************************************************
 * @fn      osal_memcmp
 *
 * @brief
 *
 *   Generic memory compare.
 *
 * @param   src1 - source 1 addrexx
 * @param   src2 - source 2 address
 * @param   len - number of bytes to compare
 *
 * @return  TRUE - same, FALSE - different
 */
uint8 osal_memcmp( const void *src1, const void *src2, unsigned int len )
{
  const uint8 *pSrc1;
  const uint8 *pSrc2;

  pSrc1 = src1;
  pSrc2 = src2;

  while ( len-- )
  {
    if( *pSrc1++ != *pSrc2++ )
      return FALSE;
  }
  return TRUE;
}

