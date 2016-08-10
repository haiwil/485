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
#define SIM900A_SEND_BUF_SIZE  1*1024  //size must be power of 2. 
u8 sim900q_send_buf[SIM900A_SEND_BUF_SIZE] = {0};
//缓冲区结构定义和初始化
struct __kfifo SIM900A_send_fifo=
{
    sim900q_send_buf,    //缓冲区
    SIM900A_SEND_BUF_SIZE,  //大小
    0,       	//入口位置
    0,        	//出口位置
};

/**************************************************************************************************
 *                                          CONSTANTS
 **************************************************************************************************/
const u8* default_TCP_server_ip="119.29.155.148";  //IP
const u8* default_TCP_server_portnum="22222"; 	   //端口
u8  SIM900A_End_Data[1] = {0x1A};
u8 Sim900a_Connect_Ok = FALSE;
/**************************************************************************************************
 *                                          TYPEDEFS
 **************************************************************************************************/


/**************************************************************************************************
 *                                      GLOBAL VARIABLES
 **************************************************************************************************/
u8  Current_AT_cmd_state;
u8  Next_AT_cmd_state;
u8  Error_AT_cmd_state;
char SIM900A_temp_cmd_buf[200];
u8   SIM900A_Data[64];
char SIM900A_Data_Len;
/**************************************************************************************************
 *										FUNCTIONS PROTOTYPES
 **************************************************************************************************/
static void SIM900A_Recv_Usart1_Data(void);
static void SIM900A_CONNECT_GPRS(void);
static void SIM900A_CheckFiFoProcess(void);
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
void SIM900A_Init( TASK task_id )
{
  /* Register task ID */
  SIM900A_TaskID = task_id;  
	Current_AT_cmd_state = TEST_SIM900A_AT_CMD_STATE;	
	osal_update_timerEx(100, SIM900A_TaskID, SIM900A_CONNECT_GPRS_EVENT);
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
EVENT SIM900A_ProcessEvent( TASK task_id, EVENT events )
{
  osal_msg_hdr_t *msgPtr;
  
  (void)task_id;  // Intentionally unreferenced parameter

  
  /*消息事件*/
  if ( events & SYS_EVENT_MSG )
  {
     msgPtr = osal_msg_receive(SJ485_TaskID);
 
     while (msgPtr) 
     {
       /* Do something here - for now, just deallocate the msg and move on */
       //SIM900A_ProcessOSALMsg( msgPtr );
 	  
       /* De-allocate */
       osal_msg_deallocate( msgPtr );
       /* Next */
       msgPtr = osal_msg_receive( SJ485_TaskID );
     }
     return events ^ SYS_EVENT_MSG;
  }

	if ( events & SIM900A_EVENT )
	{
			SIM900A_Recv_Usart1_Data();
			return events ^ SIM900A_EVENT;
	}

	if ( events & SIM900A_CONNECT_GPRS_EVENT )
	{
			SIM900A_CONNECT_GPRS();
			return events ^ SIM900A_CONNECT_GPRS_EVENT;
	}
		
	if ( events & SIM900A_SET_AT_TIME_OUT_EVENT )
	{
			Current_AT_cmd_state = Error_AT_cmd_state;
			//超时重新开始
			SIM900A_CONNECT_GPRS();
			return events ^ SIM900A_SET_AT_TIME_OUT_EVENT;
	}
	
	#if 1
	if ( events & SIM900A_PREPARE_SEND_DATA_EVENT )
	{
			UART4_Send_String("AT+CIPSEND\r\n");
			osal_update_timerEx( SIM900A_PREPARE_SEND_DATA_TIME, SIM900A_TaskID, SIM900A_SEND_DATA_EVENT);
			return events ^ SIM900A_PREPARE_SEND_DATA_EVENT;
	}
	
	if ( events & SIM900A_SEND_DATA_EVENT )
	{
			SIM900A_UART_Send_Data(SIM900A_Data,SIM900A_Data_Len);
			osal_update_timerEx( SIM900A_SEND_DATA_TIME, SIM900A_TaskID, SIM900A_SEND_END_DATA_EVENT);
			return events ^ SIM900A_SEND_DATA_EVENT;
	}
	
	if ( events & SIM900A_SEND_END_DATA_EVENT )
	{
			SIM900A_UART_Send_Data(SIM900A_End_Data,1);
			return events ^ SIM900A_SEND_END_DATA_EVENT;
	}
	
	if ( events & SIM900A_CHECK_SEND_FIFO_EVENT )
	{
			SIM900A_CheckFiFoProcess();
			return events ^ SIM900A_CHECK_SEND_FIFO_EVENT;
	}
	#endif
  /* Nothing interested, discard the message */
  return 0;

}

/**************************************************************************************************
**************************************************************************************************/
void SIM900A_Recv_Usart1_Data(void)
{
		UART4_RxDataProcess();
}

void SIM900A_CONNECT_GPRS(void)
{
	switch(Current_AT_cmd_state)	
	{
		case TEST_SIM900A_AT_CMD_STATE:
			UART4_Send_String("AT\r\n");
			//Debug_printf("AT");
			Next_AT_cmd_state = SET_MOBILE_CLASS_STATE;
			Error_AT_cmd_state = TEST_SIM900A_AT_CMD_STATE;
			osal_update_timerEx( SIM900A_SET_AT_TIME, SIM900A_TaskID, SIM900A_SET_AT_TIME_OUT_EVENT);
			break;
		case SET_MOBILE_CLASS_STATE:
			UART4_Send_String("AT+CGCLASS=\"B\"\r\n");
			//Debug_printf("AT+CGCLASS=\"B\"\r\n");
			Next_AT_cmd_state = SET_CONNECT_STYLE_STATE;
			Error_AT_cmd_state = TEST_SIM900A_AT_CMD_STATE;	
			osal_update_timerEx( SIM900A_SET_AT_TIME, SIM900A_TaskID, SIM900A_SET_AT_TIME_OUT_EVENT);
			break;
		case SET_CONNECT_STYLE_STATE:
			UART4_Send_String("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");
			Next_AT_cmd_state = SET_JOINT_STATE;
			Error_AT_cmd_state = TEST_SIM900A_AT_CMD_STATE;	
			osal_update_timerEx( SIM900A_SET_AT_TIME, SIM900A_TaskID, SIM900A_SET_AT_TIME_OUT_EVENT);
			break;
		case SET_JOINT_STATE:
			UART4_Send_String("AT+CGATT=1\r\n");
			Next_AT_cmd_state = SET_GPRS_STATE;
			Error_AT_cmd_state = TEST_SIM900A_AT_CMD_STATE;	
			osal_update_timerEx( SIM900A_SET_AT_TIME, SIM900A_TaskID, SIM900A_SET_AT_TIME_OUT_EVENT);
			break;
		case SET_GPRS_STATE:
			UART4_Send_String("AT+CIPCSGP=1,\"CMNET\"\r\n");
			Next_AT_cmd_state = SET_LOCAL_TCP_PORT_STATE;
			Error_AT_cmd_state = TEST_SIM900A_AT_CMD_STATE;
			osal_update_timerEx( SIM900A_SET_AT_TIME, SIM900A_TaskID, SIM900A_SET_AT_TIME_OUT_EVENT);
			break;
		case SET_LOCAL_TCP_PORT_STATE:
			UART4_Send_String("AT+CLPORT=\"TCP\",\"2222\"\r\n");
			Next_AT_cmd_state = SET_TCP_CONNECT_STATE;
			Error_AT_cmd_state = TEST_SIM900A_AT_CMD_STATE;	
			osal_update_timerEx( SIM900A_SET_AT_TIME, SIM900A_TaskID, SIM900A_SET_AT_TIME_OUT_EVENT);
			break;
		case SET_TCP_CONNECT_STATE:
			sprintf(SIM900A_temp_cmd_buf, "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n", default_TCP_server_ip, default_TCP_server_portnum);
			UART4_Send_String(SIM900A_temp_cmd_buf);
			Next_AT_cmd_state = SIM900A_SEND_DATA_CMD;
			Error_AT_cmd_state = TEST_SIM900A_AT_CMD_STATE;	
			osal_update_timerEx( 10*60*1000, SIM900A_TaskID, SIM900A_SET_AT_TIME_OUT_EVENT);
			break;
		case SIM900A_SEND_DATA_CMD:
			//
			break;
	}
}


void SIM900A_CheckFiFoProcess(void)
{
	u8 pkt_len;
  //if( ring_buffer_len_used(&SIM900A_send_fifo) >= SIM900A_BASIC_PKT_INFO_LEN )
	{
			ring_buffer_get(&SIM900A_send_fifo, &pkt_len, 1);
			SIM900A_Data_Len = pkt_len;
			ring_buffer_get( &SIM900A_send_fifo, SIM900A_Data, pkt_len );
			osal_set_event(SIM900A_TaskID, SIM900A_PREPARE_SEND_DATA_EVENT);
	}
}

ErrorStatus SIM900A_Send_Data(u8 *buf, u8 len)
{
	if(len==0) return INVALID_PARAM;
  if( SIM900A_MAX_PKT_LEN <= ring_buffer_len_unused( &SIM900A_send_fifo ) )
  {
    ring_buffer_put( &SIM900A_send_fifo, &len, 1 );
    ring_buffer_put( &SIM900A_send_fifo, buf, len );
    osal_update_timerEx( SIM900A_START_CHECK_SEND_FIFO_TIME, SIM900A_TaskID, SIM900A_CHECK_SEND_FIFO_EVENT);
    return SUCCESS;
  }
  else
  {
    //Debug_printf("\r\nRing Full\r\n");
		SIM900A_send_fifo.in = SIM900A_send_fifo.out = 0;//清FIFO
    return MEM_FULL;
  }
}


NetErrorStatus SIM900A_make_msg( void )
{
	u8 len;
  OsalSIM900AData_t* pkt_msg;

  pkt_msg = (OsalSIM900AData_t*)osal_msg_allocate( sizeof(OsalSIM900AData_t) );
  if( pkt_msg == NULL ) {
     return MEM_FULL;
  }  
  
	pkt_msg->msg = (u8*)pkt_msg + sizeof(OsalSIM900AData_t);
	UART4_Receive_Data(pkt_msg->msg,&len);
	pkt_msg->length = len;

  if( SUCCESS != osal_msg_send( App_TaskID, APP_SIM900A_MSG, (osal_msg_hdr_t *)pkt_msg, 0 ) )
  {
    return INVALID_PARAM;
  }
  return SUCCESS;
}


