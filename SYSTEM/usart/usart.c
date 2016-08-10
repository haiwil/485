
#include "main.h"				

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 

#if 1
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif

#endif
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
  //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_ClearFlag(USART1, USART_FLAG_TC);            //
  USART_GetFlagStatus(USART1,USART_FLAG_TC);         //  

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);/* ʹ�ܴ���1���߿����ж� */
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
  
	#ifdef Uart_Debug
	USART1_Send_String("usart1 init ok!\n");
	#endif
}



/***************************************************************************************************
 * @fn      USART1_IRQHandler
 *
 * @brief   | SOI |    Data   |  EOI  |
 *          |  1  |   0-Len   |   1   |
 *
 * @return  None
 ***************************************************************************************************/
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		USART1_Send_Data(&Res,1);
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
				if( (USART_RX_STA&0X3FFF) < (USART_REC_LEN) )   
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;	
				}
				else
				{
					USART_RX_STA=0; //���մ���,���¿�ʼ	
				}
		 }
	 }
	
	 if ( USART_GetITStatus( USART1, USART_IT_IDLE ) == SET )      //����֡�������
	 {
        USART_RX_STA|=0x8000;	//���������
				osal_SetInterruptEvent(USART_RX_FINISH_INTERRUPT_EVENT);
		
		    Res = USART_ReceiveData( USART1 );  //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR
    }

} 

/**************************************************************************************************
* @fn		Usart_SendRxMsg
*
* @brief	�Ѵ��ڽ������ݴ��Ϊ��Ϣ���͸�App_Task
*
* @param	none
*
* @return	NetErrorStatus
**************************************************************************************************/
NetErrorStatus Usart_SendRxMsg( void )
{
  u8 i = 0;
  u8 len = 0;
  OsalUSARTData_t* pkt_msg;

  if( USART_RX_STA&0x8000 )
  {
      len = USART_RX_STA&0X3FFF ;	  
	  if(len > USART_REC_LEN)
	  {
        USART_RX_STA = 0;
  	    return ERROR;
	  }
	  pkt_msg = (OsalUSARTData_t*)osal_msg_allocate( sizeof(OsalUSARTData_t) + len );
	  if( pkt_msg == NULL )
	  {
        USART_RX_STA = 0;
  	    return MEM_FULL;
	  }
	  pkt_msg->length = len;
	  pkt_msg->msg = (u8*)pkt_msg + sizeof(OsalUSARTData_t);
	  
	  for( i=0; i < len; i++ )
	  {
	  	pkt_msg->msg[i] = USART_RX_BUF[i];
	  }
      
	  #ifdef Uart_Debug
        Debug_DealSerialCMD(pkt_msg);
      #endif 
      
	  if( SUCCESS != osal_msg_send( App_TaskID, APP_SERIAL_MSG, (osal_msg_hdr_t *)pkt_msg ,0 ) )
	  {
	  	USART_RX_STA = 0;
	    return INVALID_PARAM;
	  }
	  USART_RX_STA = 0;
	  return SUCCESS;
  }
  else
  {
     return ERROR;
  }
}

#endif	

 
u8 USART1_Send_String(char* string)
{
  u8 len = strlen(string);
  USART1_Send_Data((u8*)string,len);
  return len;
}


void USART1_Send_Data(u8 *buf, u8 len)
{
	u8 t;

	for(t=0; t<len; t++)
	{
		while(!(USART1->SR&USART_FLAG_TXE))	;
		USART_SendData(USART1,buf[t]);
	}
	while(!(USART1->SR&USART_FLAG_TXE))	;
	USART_RX_STA=0;
}





#ifdef Uart_Debug
s32 msg_debug_cnt = 0;
s32 stored_pkt_cnt = 0;
s32 broadcast_cnt = 0;
s32 resend_pkt_cnt = 0;
u8 debug_on = TRUE;
#define MAX_CMD_BUF_NUMS  15
char msgCmdBuf[MAX_CMD_BUF_NUMS];
void Debug_DealSerialCMD(OsalUSARTData_t *cmdMsg)
{
  uint16 len = cmdMsg->length;
  uint8 i,j;
  if( len > MAX_CMD_BUF_NUMS) 
    return;
  
  for(i=0,j=0; i < len; i++)
  {
      if(cmdMsg->msg[i] == ' ' || cmdMsg->msg[i] == '\t'){
  
      }
      else{
          msgCmdBuf[j++] = cmdMsg->msg[i];
      }
      if( i+1 == len ) {
          msgCmdBuf[j] = '\0';
      }
  }
  if( strcmp( msgCmdBuf, "DebugOn" ) == 0 ){
    debug_on = TRUE;
    //Debug_printf("Debug On.\r\n");
  }
  else if( strcmp( msgCmdBuf, "DebugOff" ) == 0 ){
   debug_on = FALSE;
  }
  else if( strcmp( msgCmdBuf, "MemUsed" ) == 0 ){
   printf("LinkedListMemPerused:%d\r\n", LinkedListMemPerused());
   printf("TimerEventListMemPerused:%d\r\n", TimerEventListMemPerused());
  }
  else {
    //Debug_printf("Error cmd.\r\n");
  }
}

#endif
