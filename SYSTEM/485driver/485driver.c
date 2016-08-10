
#include "main.h"

#ifdef EN_USART2_RX //���ʹ���˽���
//���ջ�����
u8 RS485_RX_BUF[USART2_MAX_RECV_LEN]={0};
//���յ������ݳ���
u8 RS485_RX_CNT=0;

u8 USART2_Send_String(char* string);
void USART2_Send_Data(u8* data, u16 len);

void USART2_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)//��������
	{
		res = USART_ReceiveData(USART2);//��ȡ��������
		//Debug_printf("%c",res);
		if(RS485_RX_CNT<USART2_MAX_RECV_LEN)
		{
				RS485_RX_BUF[RS485_RX_CNT] = res;
				RS485_RX_CNT++;
		}
		else//���մ���
		{
			RS485_RX_CNT = 0;
		}
	}
	
	if ( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )      //����֡�������
	{
        if((RS485_RX_CNT&0x8000)==0)//����δ���
				{
            RS485_RX_CNT|=0x8000;	//���������
						osal_SetInterruptEvent(SJ485_INTERRUPT_EVENT);
        }		
				res = USART_ReceiveData( USART2 );  //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR
  }
}
#endif

//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void RS485_Init(u32 bound)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//USART2_TX   PA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2
   
    //USART2_RX	  PA.3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3
	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//��λ����2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//ֹͣ��λ
	
	#ifdef EN_USART2_RX
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//�ֳ�Ϊ9λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_Even;//żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������	2  	  
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	/* ʹ�ܴ���2�����ж� */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	/* ʹ�ܴ���2���߿����ж� */
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 	
	#endif
	
	#ifdef Uart_Debug
	//RS485_Send_Data("init 485 ok!!!\n",15);
	#endif
}

//RS485����len���ֽ�
//buf �������׵�ַ
//len ���͵��ֽ���
void RS485_Send_Data(u8 *buf, u8 len)
{
	u8 t;
	for(t=0; t<len; t++)
	{
		while(!(USART2->SR&USART_FLAG_TXE))	;
		USART_SendData(USART2,buf[t]);
	}
	while(!(USART2->SR&USART_FLAG_TXE))	;
	RS485_RX_CNT=0;
}

void RS485_Receive_Data(u8 *buf, u16 *len)
{
	u8 rxlen = RS485_RX_CNT;
	u8 i = 0;
	*len = 0;//Ĭ��Ϊ0
	delay_ms(10);//�ȴ�10ms������10msû�н��յ����ݣ�����Ϊ���ս���
	if(rxlen == RS485_RX_CNT&& rxlen)//���յ������ҽ�������
	{
		for(i=0; i<rxlen; i++)
		{
			buf[i] = RS485_RX_BUF[i];
		}
		*len = RS485_RX_CNT;//��¼�������ݳ���
		RS485_RX_CNT=0;//����
	}
	
}

void Reset_485(void)
{
	// Initialize the Memory Allocation System
  LinkedListMemInit();
  TimerEventListMemInit();
	// Initialize the message queue
	osal_msg_unit();
	osal_clear_event(SJ485_TaskID, 0xFFFF);
	osal_clear_event(App_TaskID, 0xFFFF);    
  SIM900A_send_fifo.in = SIM900A_send_fifo.out = 0;//��FIFO
	RS485_Init(9600);
	
	osal_update_timerEx( 100, Hal_TaskID, HAL_LED_BLINK_EVENT);
	osal_update_timerEx( 100, App_TaskID, APP_SIM900A_HEART_BEAT_EVENT);
}

