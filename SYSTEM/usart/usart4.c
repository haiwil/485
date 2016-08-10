
#include "main.h"

//���ջ�����
u8 USART4_RX_BUF[USART4_MAX_RECV_LEN]={0};
//���յ������ݳ���
u8 USART4_RX_CNT=0;



void UART4_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(UART4,USART_IT_RXNE) != RESET)//��������
	{
		res = USART_ReceiveData(UART4);//��ȡ��������
		Debug_printf("%c",res);
		if(USART4_RX_CNT<USART4_MAX_RECV_LEN)
		{
				USART4_RX_BUF[USART4_RX_CNT] = res;
				USART4_RX_CNT++;
		}
		else//���մ���
		{
			USART4_RX_CNT = 0;
		}
	}
	
	if ( USART_GetITStatus( UART4, USART_IT_IDLE ) == SET )      //����֡�������
	{
        if((USART4_RX_CNT&0x8000)==0)//����δ���
				{
            USART4_RX_CNT|=0x8000;	//���������
						osal_SetInterruptEvent(SIM900A_INTERRUPT_EVENT);
        }		
				res = USART_ReceiveData( UART4 );  //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR
  }
}


//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void UART4_Init(u32 bound)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	// GPIOCʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);

 	//USART_DeInit(UART4);  //��λ����2
	
		 //USART2_TX   PC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��PA2
   
    //USART2_RX	  PC.11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��PA3
	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4,ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4,DISABLE);

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(UART4, &USART_InitStructure); //��ʼ������	2  	  
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	/* ʹ�ܴ���2�����ж� */
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	/* ʹ�ܴ���2���߿����ж� */
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
	USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ��� 	
	
}

//RS485����len���ֽ�
//buf �������׵�ַ
//len ���͵��ֽ���
void UART4_Send_Data(u8 *buf, u8 len)
{
	u8 t;
	for(t=0; t<len; t++)
	{
		while(!(UART4->SR&USART_FLAG_TXE))	;
		USART_SendData(UART4,buf[t]);
	}
	while(!(UART4->SR&USART_FLAG_TXE))	;
	USART4_RX_CNT=0;
}

u8 UART4_Send_String(char* string)
{
  u8 len = strlen(string);
  UART4_Send_Data((u8*)string,len);
  return len;
}

void UART4_Receive_Data(u8 *buf, u8 *len)
{
	u8 rxlen = USART4_RX_CNT;
	u8 i = 0;
	*len = 0;//Ĭ��Ϊ0
	delay_ms(10);//�ȴ�10ms������10msû�н��յ����ݣ�����Ϊ���ս���
	if(rxlen == USART4_RX_CNT&& rxlen)//���յ������ҽ�������
	{
		for(i=0; i<rxlen; i++)
		{
			buf[i] = USART4_RX_BUF[i];
		}
		*len = USART4_RX_CNT;//��¼�������ݳ���
		USART4_RX_CNT=0;//����
	}
	
}

void UART4_RxDataProcess(void)
{  	
		//Debug_printf("%s",USART4_RX_BUF);
		switch(Current_AT_cmd_state)
		{
			case TEST_SIM900A_AT_CMD_STATE:
			case SET_MOBILE_CLASS_STATE:
			case SET_CONNECT_STYLE_STATE:	
				 if( strstr((const char*)USART4_RX_BUF,"OK"))
				 {
							Current_AT_cmd_state = Next_AT_cmd_state;
							osal_delete_timerEx(SIM900A_TaskID, SIM900A_SET_AT_TIME_OUT_EVENT);
							osal_update_timerEx(50, SIM900A_TaskID, SIM900A_CONNECT_GPRS_EVENT);
				 }
				break;
			case SET_GPRS_STATE:	 
			case SET_JOINT_STATE:	
			case SET_LOCAL_TCP_PORT_STATE:	
				 if( strstr((const char*)USART4_RX_BUF,"OK") || strstr((const char*)USART4_RX_BUF,"ERROR"))
				 {
							Current_AT_cmd_state = Next_AT_cmd_state;
							osal_delete_timerEx(SIM900A_TaskID, SIM900A_SET_AT_TIME_OUT_EVENT);
							osal_update_timerEx(50, SIM900A_TaskID, SIM900A_CONNECT_GPRS_EVENT);
				 }
				break;	 
			case SET_TCP_CONNECT_STATE:
				 if( strstr((const char*)USART4_RX_BUF,"CONNECT OK") || strstr((const char*)USART4_RX_BUF,"ALREADY CONNECT"))
				 {
							Current_AT_cmd_state = Next_AT_cmd_state;							
							Sim900a_Connect_Ok = TRUE;
							//���͵�APP�� ���������������ͨ��
							SIM900A_make_msg();
							osal_delete_timerEx(SIM900A_TaskID, SIM900A_SET_AT_TIME_OUT_EVENT);
							//SIM900A_Send_Data("hello\r\n",6);
				 }
				 else if(strstr((const char*)USART4_RX_BUF,"CONNECT FAIL") || strstr((const char*)USART4_RX_BUF,"ERROR"))
				 {
						  Current_AT_cmd_state = Error_AT_cmd_state;
							osal_delete_timerEx(SIM900A_TaskID, SIM900A_SET_AT_TIME_OUT_EVENT);
							osal_update_timerEx(50, SIM900A_TaskID, SIM900A_CONNECT_GPRS_EVENT);
				 }
				break;
			case SIM900A_SEND_DATA_CMD:
				if( strstr((const char*)USART4_RX_BUF,">"))
				 {
							osal_delete_timerEx(SIM900A_TaskID, SIM900A_SET_AT_TIME_OUT_EVENT);
				 }
				break;	 
		}
	
}

