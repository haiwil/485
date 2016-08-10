/**************************************************************************************************
  Filename:       hal.h
  Revised:        $Date: 2016-1-8  $
  Revision:       $Revision: 00001 $

  Description:    This file contains the hal stack definitions.

**************************************************************************************************/
#ifndef __SIM900A_H
#define __SIM900A_H


/*********************************************************************
 * INCLUDES
 */
#include "types.h"
#include "485driver.h"
/*********************************************************************
 * CONSTANTS
 */


#define     SIM900A_UART_Send_Data                			UART4_Send_Data
/*********************************************************************
 * TYPEDEFS
 */
 	


/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern struct __kfifo SIM900A_send_fifo;
extern u8  Sim900a_Connect_Ok;
extern u8  Current_AT_cmd_state;
extern u8  Next_AT_cmd_state;
extern u8  Error_AT_cmd_state;
/*********************************************************************
 * MACROS
 */
#define SIM900A_MAX_PKT_LEN									512 
#define SIM900A_BASIC_PKT_INFO_LEN					21 
 
#define SIM900A_SET_AT_TIME									500
#define SIM900A_PREPARE_SEND_DATA_TIME			1500
#define SIM900A_SEND_DATA_TIME							100
#define SIM900A_START_CHECK_SEND_FIFO_TIME	1000
/*................................................WIFI AT CMD state................................................*/
#define TEST_SIM900A_AT_CMD_STATE           			1  //发送:AT  成功回复:OK
#define SET_MOBILE_CLASS_STATE										2  //发送:AT+CGCLASS="B" 成功回复:OK
#define SET_CONNECT_STYLE_STATE										3	 //发送:AT+CGDCONT=1,"IP","CMNET" 成功回复:OK
#define SET_JOINT_STATE														4  //发送:AT+CGATI=1 成功回复:OK
#define SET_GPRS_STATE														5  //发送:AT+CIPCSGP=1,"CMNET" 成功回复:OK
#define SET_LOCAL_TCP_PORT_STATE									6  //发送:AT+CLPORT="TCP","2000" 成功回复:OK
//发送:AT+CIPSTART="TCP","119.29.155.148","8080" 
//成功回复:
//OK 
//CONNECT OK
#define SET_TCP_CONNECT_STATE											7  
#define SIM900A_SEND_DATA_CMD											8
/*********************************************************************
 * FUNCTIONS
 */

void SIM900A_Init( TASK task_id );

EVENT SIM900A_ProcessEvent( TASK task_id, EVENT events );
extern ErrorStatus SIM900A_Send_Data(u8 *buf, u8 len);
extern NetErrorStatus SIM900A_make_msg( void );
#endif


