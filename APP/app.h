/**************************************************************************************************
  Filename:       app.h
  Revised:        $Date: 2016-1-7  $
  Revision:       $Revision: 00001 $

  Description:    This file contains the Sample Application definitions.

**************************************************************************************************/
#ifndef __APP_H
#define __APP_H


/*********************************************************************
 * INCLUDES
 */
#include "types.h"
#include "main.h"

#define HEART_BEAT_TIME								20000
/*********************************************************************
 * CONSTANTS
 */
#define MAX_RX_FRAME_LENGTH           128
//RS485通信协议
#define RS485_SYN											0xAA
#define RS485_EOT											0xAF
#define RS485_DLE											0xA0
#define RS485_SOH											0xE0
#define RS485_BCSOH										0xBB
#define RS485_EXT											0xE1
#define RS485_ACK											0xE2
#define RS485_NAK											0xE4
#define RS485_NUL											0xE8
#define RS485_NULACK									0xE9
#define RS485_ENQ											0xE7
#define RS485_SAK											0xD0
#define RS485_SAKED										0xDF
#define RS485_LINK										0xF1
#define RS485_LINKED									0xF4
#define RS485_UNLINK									0xF2
#define RS485_UNLINKED								0xF8

//SIM900A通信协议
#define SIM900A_HEART_BEAT						0x01
#define SIM900A_ALARM									0x02
#define SIM900A_ACK										0x99

enum RS485_RX_packageOffset
{
	RS485_SYN_OFFSET = 0,
	RS485_CHARACTER_OFFSET = 4,
	RS485_SRC_ADDR_OFFSET = 5,    //主机源
	RS485_DEST_ADDR_OFFSET =6,
};

enum RS485_BC
{
	RS485_BC_RESET = 0x00,
	RS485_BC_CANCEL_VOICE = 0x01,
	RS485_BC_SYNC_TIME = 0x0E
};

enum SIM900A_TX_packageOffset
{
	SIM900A_TX_HEAD_OFFSET =0,
	SIM900A_TX_CMD1_OFFSET =1,
	SIM900A_TX_CMD2_OFFSET =2,
	SIM900A_TX_SEQ_OFFSET  =3,
	SIM900A_TX_DATA_LEN_OFFSET =5
};

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

void App_Init( TASK task_id );

EVENT App_ProcessEvent( TASK task_id, EVENT events );

#endif
