 
#ifndef TYPES_H 
#define TYPES_H


/* takes a byte out of a uint32 : var - uint32,  ByteNum - byte to take out (0 - 3) */
#define BREAK_UINT32( var, ByteNum ) \
          (uint8)((uint32)(((var) >>((ByteNum) * 8)) & 0x00FF))

#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
          ((uint32)((uint32)((Byte0) & 0x00FF) \
          + ((uint32)((Byte1) & 0x00FF) << 8) \
          + ((uint32)((Byte2) & 0x00FF) << 16) \
          + ((uint32)((Byte3) & 0x00FF) << 24)))

#define BUILD_UINT16(loByte, hiByte) \
          ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

#define BUILD_UINT8(hiByte, loByte) \
          ((uint8)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))

#define HI_UINT8(a) (((a) >> 4) & 0x0F)
#define LO_UINT8(a) ((a) & 0x0F)
//////////////////////////////////////////////////////////
//hal types
typedef u32  INT32U;
typedef u16  INT16U;
typedef u8   INT8U;
typedef u8   u_char;

typedef u32 U32;
typedef u16 U16;
typedef u8  U8;

//typedef u32 halIntState_t;

//////////////////////////////////////////////////////////
//os types
typedef ErrorStatus    NetErrorStatus;
typedef u8    uint8;
typedef u16   uint16;
typedef u32   uint32;
typedef s8    int8;
typedef s16    int16;
typedef s32    int32;

typedef u8    state_t;
typedef u16 TASK;
typedef u16 EVENT;
typedef u32 TIMESTAMP;
typedef u32 inter_event_t;

//////////////////////////////////////////////////////////
//net types
#define NET_MAX_NODE  1024
#define NET_MAC_LENGTH 4

#define TRUE   1
#define FALSE (!TRUE)

typedef ErrorStatus    OsalErrorStatus;
typedef u8   Hitch_Alarm_type_t;
typedef u8   DevID_t;
typedef u8   NET_MAC_t;
typedef u8   PANID_t;
typedef u8   CHANNEL_t;
typedef u32  NET_TIMER_t;
typedef u8   NET_RT_LEN_t;
typedef u16  NET_NODE_ID_t;
typedef u16  NET_SLEEP_NODE_t;

/*********************************************************************
 * MSG TYPEDEFS
 */


typedef enum
{
  NWK_INIT,
  NWK_DISC,
  NWK_SETUPING,
  NWK_SUSPEND,
  NWK_JOINING,
  NWK_ENDDEVICE,
  NWK_ROUTER,
  NWK_COORDINATOR
} nwk_states_t; 


typedef struct
{
  void          *next;
  TIMESTAMP     timestamp;
  TASK          dest_id;
  EVENT         event;
  uint16        len;
} osal_msg_hdr_t;


typedef void* osal_msg_q_t;

typedef struct
{
  osal_msg_hdr_t msg_hdr;     /* OSAL Message memory header */
  uint8 state; // shift
  uint8 keys;  // keys
} keyChange_t;

typedef struct
{
  osal_msg_hdr_t msg_hdr;     /* OSAL Message memory header */
  nwk_states_t state; // shift
} nwk_stateMSG_t;


typedef struct
{
  osal_msg_hdr_t msg_hdr;     /* OSAL Message memory header */
  u8  length;
  u8   *msg;
} OsalUSARTData_t;

typedef struct
{
  osal_msg_hdr_t msg_hdr;     /* OSAL Message memory header */
  u16  length;
  u8   *msg;
} OsalSJ485Data_t;

typedef struct
{
  osal_msg_hdr_t msg_hdr;     /* OSAL Message memory header */
  u8  length;
  u8   *msg;
} OsalSIM900AData_t;

#define OSAL_USARTDATA_LEN(msg_ptr)       ((OsalUSARTData_t *)msg_ptr)->length
#define OSAL_USARTDATA_MSGp(msg_ptr)      ((OsalUSARTData_t *)msg_ptr)->msg

typedef struct
{
  osal_msg_hdr_t 		hdr;     /* OSAL Message header */
  NET_NODE_ID_t 		StartAddr;          //
  NET_NODE_ID_t 		DestAddr;           //
  u8                    cmd;
  PANID_t 				PanID;              //
  u8            		rssi;               //
  u8 					Seq;
  u8					Payload_len;
  u8*					Payload;
} appIncomingMSGPacket_t;

typedef struct
{
  osal_msg_hdr_t msg_hdr;     /* OSAL Message memory header */
  uint8  rssi;
  uint8  DataLength;              // Number of bytes in TransData
  PANID_t   panId;  // used for the INTER_PAN feature
  NET_NODE_ID_t  TargetAddr;
  NET_NODE_ID_t  SourceAddr;
  uint8  Seq;
  uint8   cmd;
  uint8  *Data;
} RFIncomingMSGPacket_t;

typedef struct
{
  osal_msg_hdr_t msg_hdr;     /* OSAL Message memory header */
  NET_NODE_ID_t  TargetAddr;
  uint8  Seq;
} MAC_WAIT_ACK_TIMEOUT_MSG_t;

#endif
