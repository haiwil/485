/**************************************************************************************************
  Filename:       app.c
  Revised:        $Date: 2016-1-7 $
  Revision:       $Revision: 00001 $

  Description:    app Stack.
  */


/*********************************************************************
 * INCLUDES
 */

#include "main.h"

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * TYPEDEFS
 */



/*********************************************************************
 * GLOBAL VARIABLES
 */
//485
u16 send_len;
u8 pFrame[MAX_RX_FRAME_LENGTH];
u8 PKG_NO=0;
u8 RECV_PKG_NO=0;
u8 App_Protecl_Layer_Type = 0;
u8 Is_Send_Data_to_Master = 0;//0 - no  1 - yes
//SIM900A
u8 SIM900A_Pframe[MAX_RX_FRAME_LENGTH];
u8 SIM900A_SEQ=0;
u8 RS485_CMD;
u8 RS485_DATA[MAX_RX_FRAME_LENGTH];
u8 RS485_DATA_LEN;
u8 SIM900A_MAC[4]={0xaa,0xbb,0xcc,0xdd};
/*********************************************************************
 * EXTERNAL VARIABLES
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static u8 RS485_RX_buffer[MAX_RX_FRAME_LENGTH] = {0};
/*********************************************************************
 * LOCAL FUNCTIONS
 */
static u8 Fill_RS485_Package( u8 character, u8 *pFrame ); // pData is input, pFrame is output
static u8 Fill_Sim900a_Package( u8 cmd1, u8 cmd2, u8 *pFrame ); // pData is input, pFrame is output
static void SampleApp_HandleKeys( uint8 shift, uint8 keys );
static u8 SampleApp_SJ485CMD(OsalSJ485Data_t *MSGpkt);
/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void App_Init( TASK task_id )
{
  App_TaskID = task_id;   
}


EVENT App_ProcessEvent( TASK task_id, EVENT events )
{
   osal_msg_hdr_t *MSGpkt;

  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & APP_SIM900A_HEART_BEAT_EVENT)
  {
		send_len = Fill_Sim900a_Package(0x0E,SIM900A_HEART_BEAT,SIM900A_Pframe);
		SIM900A_Send_Data(SIM900A_Pframe,send_len);
    osal_update_timerEx( HEART_BEAT_TIME, App_TaskID, APP_SIM900A_HEART_BEAT_EVENT);
    return ( events ^ APP_SIM900A_HEART_BEAT_EVENT);
  }
   
	if (events & APP_RS485_LED_BLINK_EVENT)
  {  
		LED3=!LED3;
  	//osal_update_timerEx( 1000, App_TaskID, APP_RS485_LED_BLINK_EVENT);
    return events ^ APP_RS485_LED_BLINK_EVENT;
  }
	
  /*消息事件*/
  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (osal_msg_hdr_t *)osal_msg_receive( App_TaskID );
    while ( MSGpkt )
    {
      switch ( OSAL_MSG_EVT(MSGpkt) )
      {
        // Received when a key is pressed
        case APP_KEY_CHANGE_MSG:
          SampleApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;
          
        case APP_SERIAL_MSG:
          //SampleApp_SerialCMD((OsalUSARTData_t *)MSGpkt);
          break;

				case APP_SJ485_MSG:
					//if(TRUE == Sim900a_Connect_Ok)//保证GPRS连上在485通信
					{
							SampleApp_SJ485CMD((OsalSJ485Data_t *)MSGpkt);
					}
					break;
				
				case APP_SIM900A_MSG:
          //Debug_printf("GPRS Connect ok!!! Ready to send heart beat data!!!\n");
					osal_update_timerEx( 100, App_TaskID, APP_SIM900A_HEART_BEAT_EVENT);
          break;
        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (osal_msg_hdr_t *)MSGpkt );

      // Next - if one is available
      MSGpkt = osal_msg_receive( App_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  
  // Discard unknown events
  return 0;
}


/*********************************************************************
 * LOCAL FUNCTIONS
 */
 
/*********************************************************************
 * Basic Functions
 */


/*********************************************************************
 * @fn      SampleApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys )
{
  (void)shift;  // Intentionally unreferenced parameter
  
//  if ( keys == KEY0_PRES )
  {
    LED1=!LED1;
  }

//  if ( keys == KEY1_PRES )
  {
    
  }

}

//接收三江主机通过485发送过来的数据并解析
u8 SampleApp_SJ485CMD(OsalSJ485Data_t *cmdMsg)
{
	u8 i;
	u16 len = cmdMsg->length;
	
	if(0 == len) //没有收到数据说明485通信失败
	{
		//Debug_printf("%s\n","485 failure");
		Reset_485();
		return 0;
	}
	if(len > USART2_MAX_RECV_LEN) 
	{
		//Debug_printf("%s\n","485 failure");
		Reset_485();
		return 0;
	}
	for(i = 0;i < len;i++)
  {
    RS485_RX_buffer[i] = cmdMsg->msg[i];
  }
	
	osal_update_timerEx( 500, App_TaskID, APP_RS485_LED_BLINK_EVENT);
	if(((0xAA == RS485_RX_buffer[0]) && (0xAA == RS485_RX_buffer[1]) && 
		   (0xAA == RS485_RX_buffer[2]) && (0xAA == RS485_RX_buffer[3])))
	{
				#ifdef Uart1_Debug 
				for(i = 0;i < len;i++)
						Debug_printf("%02X",RS485_RX_buffer[i]);
				Debug_printf("\n");
				#endif
				//if( RS485_RX_buffer[len-1] == BCC_XOR(RS485_RX_buffer,len-2) )//异或校验结束符不参与
				{
						switch(RS485_RX_buffer[RS485_CHARACTER_OFFSET])
						{
							//--------------火警信息传输-----------------
							case RS485_LINK:
								send_len = Fill_RS485_Package(RS485_LINK,pFrame);
								if(send_len > USART2_MAX_RECV_LEN) 
								{
									Reset_485();
									return 0;
								}
								RS485_Send_Data(pFrame,send_len);
								#ifdef Uart1_Debug 
								for(i = 0;i < send_len;i++)
										Debug_printf("%02X",pFrame[i]);
								Debug_printf("\n");
								#endif 
								break;
							case RS485_ENQ:
								PKG_NO = 0;
								RECV_PKG_NO = 0;
								send_len = Fill_RS485_Package(RS485_ENQ,pFrame);
								if(send_len > USART2_MAX_RECV_LEN) 
								{
									Reset_485();
									return 0;
								}
								RS485_Send_Data(pFrame,send_len);
								#ifdef Uart1_Debug 
								for(i = 0;i < send_len;i++)
										Debug_printf("%02X",pFrame[i]);
								Debug_printf("\n");
								#endif 
								break;
							case RS485_SOH://接收到主机的火警数据 至于第几包通过RS485_RX_buffer[RS485_DEST_ADDR_OFFSET+1] = PKG_NO来判断
								if( ++RECV_PKG_NO == RS485_RX_buffer[RS485_DEST_ADDR_OFFSET+1])
								{
										send_len = Fill_RS485_Package(RS485_SOH,pFrame);
										if(send_len > USART2_MAX_RECV_LEN) 
										{
											Reset_485();
											return 0;
										}
										RS485_Send_Data(pFrame,send_len);
										#ifdef Uart1_Debug 
										for(i = 0;i < send_len;i++)
												Debug_printf("%02X",pFrame[i]);
										Debug_printf("\n");
										#endif 
										
										#if 1
										//转发GPRS数据
										RS485_DATA_LEN = RS485_RX_buffer[RS485_DEST_ADDR_OFFSET+2];
										RS485_CMD = RS485_RX_buffer[RS485_DEST_ADDR_OFFSET+3];//数据类型
										Debug_printf("RS485_CMD = %02X   ",RS485_CMD);
										Debug_printf("RS485_DATA_LEN = %02X",RS485_DATA_LEN);
										Debug_printf("\n");
										for(i = 0;i < RS485_DATA_LEN;i++)
										{
											RS485_DATA[i] = RS485_RX_buffer[RS485_DEST_ADDR_OFFSET+3+i];
											Debug_printf("%02X",RS485_DATA[i]);
										}
										Debug_printf("\n");
										send_len = Fill_Sim900a_Package(0x0E,SIM900A_ALARM,SIM900A_Pframe);
										if(send_len > USART4_MAX_RECV_LEN) return 0;
										if(MEM_FULL == SIM900A_Send_Data(SIM900A_Pframe,send_len))
										{
											return 0;
										}
										#endif
								}
								break;
							case RS485_NUL:
								send_len = Fill_RS485_Package(RS485_NUL,pFrame);
								if(send_len > USART2_MAX_RECV_LEN) 
								{
									Reset_485();
									return 0;
								}
								RS485_Send_Data(pFrame,send_len);
								#ifdef Uart1_Debug
								for(i = 0;i < send_len;i++)
										Debug_printf("%02X",pFrame[i]);
								Debug_printf("\n");
								#endif 
								break;
							case RS485_ACK:
								//有数据上传1 无数据上传0
								Is_Send_Data_to_Master = 0;//need modify
								if(send_len > USART2_MAX_RECV_LEN) 
								{
									Reset_485();
									return 0;
								}
								send_len = Fill_RS485_Package(RS485_ACK,pFrame);
								RS485_Send_Data(pFrame,send_len);
								#ifdef Uart1_Debug
								for(i = 0;i < send_len;i++)
										Debug_printf("%02X",pFrame[i]);
								Debug_printf("\n");
								#endif 
								break;
							case RS485_UNLINK:
								send_len = Fill_RS485_Package(RS485_UNLINK,pFrame);
								if(send_len > USART2_MAX_RECV_LEN) 
								{
									Reset_485();
									return 0;
								}
								RS485_Send_Data(pFrame,send_len);
								#ifdef Uart1_Debug 
								for(i = 0;i < send_len;i++)
										Debug_printf("%02X",pFrame[i]);
								Debug_printf("\n");
								#endif 
								break;
							//--------------应用层协议握手命令-----------------
							case RS485_SAK:
								App_Protecl_Layer_Type = 1;//经验证必须为1才能握手成功
								send_len = Fill_RS485_Package(RS485_SAK,pFrame);
								if(send_len > USART2_MAX_RECV_LEN) 
								{
									Reset_485();
									return 0;
								}
								RS485_Send_Data(pFrame,send_len);
								#ifdef Uart1_Debug
								for(i = 0;i < send_len;i++)
										Debug_printf("%02X",pFrame[i]);
								Debug_printf("\n");
								#endif 
								break;
							//----------------广播命令(不需要回复执行即可)------------
							case RS485_BCSOH:
								if(RS485_BC_RESET == RS485_RX_buffer[RS485_DEST_ADDR_OFFSET+1])
								{
										//do somthing
								}
								else if(RS485_BC_CANCEL_VOICE == RS485_RX_buffer[RS485_DEST_ADDR_OFFSET+1])
								{
										//do somthing
								}
								else if(RS485_BC_SYNC_TIME == RS485_RX_buffer[RS485_DEST_ADDR_OFFSET+1])
								{
										//do somthing
								}
								else
								{
										//do nothing
								}
								break;
							default:
								return 0;
						}
				}
	}
	else
	{
		//Debug_printf("%s\n","485 failure");
		Reset_485();
		return 0;
	}
	return 1;
}
		

u8 Fill_RS485_Package( u8 character, u8 *pFrame ) // pData is input, pFrame is output
{
		u8 CRC8;
		u8 packet_len = 0;
	 
		pFrame[packet_len++] = RS485_SYN;
		pFrame[packet_len++] = RS485_SYN;
		pFrame[packet_len++] = RS485_SYN;
		pFrame[packet_len++] = RS485_SYN;
		switch(character)
		{
			case RS485_LINK://火警信息传输命令
				pFrame[packet_len++] = RS485_LINKED;
				pFrame[packet_len++] = RS485_RX_buffer[RS485_DEST_ADDR_OFFSET];//主机目的地址=从机源地址
				pFrame[packet_len++] = 0x00;
				pFrame[packet_len++] = 0x00;
				break;
			case RS485_ENQ://火警信息传输命令
				pFrame[packet_len++] = RS485_ACK;
				pFrame[packet_len++] = RS485_RX_buffer[RS485_DEST_ADDR_OFFSET];
				pFrame[packet_len++] = 0x00;
				pFrame[packet_len++] = 0x00;
				break;
			case RS485_SOH://火警信息传输命令
				pFrame[packet_len++] = RS485_ACK;
				pFrame[packet_len++] = RS485_RX_buffer[RS485_DEST_ADDR_OFFSET];
				pFrame[packet_len++] = 0x00;
				pFrame[packet_len++] = ++PKG_NO;
				break;
			case RS485_NUL://火警信息传输命令
				pFrame[packet_len++] = RS485_NULACK;
				pFrame[packet_len++] = RS485_RX_buffer[RS485_DEST_ADDR_OFFSET];
				pFrame[packet_len++] = 0x00;
				pFrame[packet_len++] = 0x00;
				break;
			case RS485_ACK://火警信息传输命令
				if(0 == Is_Send_Data_to_Master) //无信息上报或信息上报完回复RS485_EXT
				{
					pFrame[packet_len++] = RS485_EXT;
					pFrame[packet_len++] = RS485_RX_buffer[RS485_DEST_ADDR_OFFSET];
					pFrame[packet_len++] = 0x00;
					pFrame[packet_len++] = 0x00;
				}
				else//if 有信息上报回复RS485_SOH  ---待修改
				{
					pFrame[packet_len++] = RS485_SOH;
					pFrame[packet_len++] = RS485_RX_buffer[RS485_DEST_ADDR_OFFSET];
					pFrame[packet_len++] = 0x00;
					pFrame[packet_len++] = PKG_NO++;
					pFrame[packet_len++] = 0x71;
					pFrame[packet_len++] = 0x05;
					pFrame[packet_len++] = 0x01;
				}
				break;
			case RS485_UNLINK://火警信息传输命令
				pFrame[packet_len++] = RS485_UNLINKED;
				pFrame[packet_len++] = RS485_RX_buffer[RS485_DEST_ADDR_OFFSET];
				pFrame[packet_len++] = 0x00;
				pFrame[packet_len++] = 0x00;
				break;
			case RS485_SAK://应用层协议握手命令
				pFrame[packet_len++] = RS485_SAKED;
				pFrame[packet_len++] = RS485_RX_buffer[RS485_DEST_ADDR_OFFSET];
				pFrame[packet_len++] = 0x00;
				pFrame[packet_len++] = App_Protecl_Layer_Type;
				break;
			default:
				return 0;
		}
		CRC8 = BCC_XOR(pFrame,packet_len);//异或校验结束符不参与
		pFrame[packet_len++] = RS485_EOT;
		pFrame[packet_len++] = CRC8;
		return packet_len;
}

u8 Fill_Sim900a_Package( u8 cmd1, u8 cmd2, u8 *pFrame ) // pData is input, pFrame is output
{
		u8 i;
		u8 CRC16;
		u8 packet_len = 0;
	 
		pFrame[packet_len++] = 0x7E;
		pFrame[packet_len++] = 0x0E;
		SIM900A_SEQ++;
		switch(cmd2)
		{
			case SIM900A_HEART_BEAT:
				pFrame[packet_len++] = SIM900A_HEART_BEAT;
				pFrame[packet_len++] = ((SIM900A_SEQ)&0xFF00)>>8;
				pFrame[packet_len++] = (SIM900A_SEQ)&0x00FF;
				pFrame[packet_len++] = 4;
				pFrame[packet_len++] = SIM900A_MAC[0];
				pFrame[packet_len++] = SIM900A_MAC[1];
				pFrame[packet_len++] = SIM900A_MAC[2];
				pFrame[packet_len++] = SIM900A_MAC[3];
				break;
			case SIM900A_ALARM:
				pFrame[packet_len++] = SIM900A_HEART_BEAT;
				pFrame[packet_len++] = ((SIM900A_SEQ)&0xFF00)>>8;
				pFrame[packet_len++] = (SIM900A_SEQ)&0x00FF;
				pFrame[packet_len++] = 6+RS485_DATA_LEN;
				pFrame[packet_len++] = SIM900A_MAC[0];
				pFrame[packet_len++] = SIM900A_MAC[1];
				pFrame[packet_len++] = SIM900A_MAC[2];
				pFrame[packet_len++] = SIM900A_MAC[3];
				pFrame[packet_len++] = RS485_CMD;
				pFrame[packet_len++] = RS485_DATA_LEN;
				for(i = 0;i < RS485_DATA_LEN;i++)
					pFrame[i + packet_len++] = RS485_DATA[i];
				break;
			case SIM900A_ACK:
				pFrame[packet_len++] = SIM900A_HEART_BEAT;
				pFrame[packet_len++] = ((SIM900A_SEQ)&0xFF00)>>8;
				pFrame[packet_len++] = (SIM900A_SEQ)&0x00FF;
				pFrame[packet_len++] = 4;
				pFrame[packet_len++] = SIM900A_MAC[0];
				pFrame[packet_len++] = SIM900A_MAC[1];
				pFrame[packet_len++] = SIM900A_MAC[2];
				pFrame[packet_len++] = SIM900A_MAC[3];
				break;
			default:
				return 0;
		}
		CRC16 = cyg_crc16(pFrame + SIM900A_TX_DATA_LEN_OFFSET, 
														packet_len-5 );//5=head+cm1+cm2+seq
		pFrame[packet_len++] = LO_UINT16(CRC16);
		pFrame[packet_len++] = HI_UINT16(CRC16);
		pFrame[packet_len++] = 0x7F;
		
		return packet_len;
}


