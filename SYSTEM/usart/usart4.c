
#include "main.h"

//接收缓存区
u8 USART4_RX_BUF[USART4_MAX_RECV_LEN]={0};
//接收到的数据长度
u8 USART4_RX_CNT=0;



void UART4_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(UART4,USART_IT_RXNE) != RESET)//接收数据
	{
		res = USART_ReceiveData(UART4);//读取接收数据
		Debug_printf("%c",res);
		if(USART4_RX_CNT<USART4_MAX_RECV_LEN)
		{
				USART4_RX_BUF[USART4_RX_CNT] = res;
				USART4_RX_CNT++;
		}
		else//接收错误
		{
			USART4_RX_CNT = 0;
		}
	}
	
	if ( USART_GetITStatus( UART4, USART_IT_IDLE ) == SET )      //数据帧接收完毕
	{
        if((USART4_RX_CNT&0x8000)==0)//接收未完成
				{
            USART4_RX_CNT|=0x8000;	//接收完成了
						osal_SetInterruptEvent(SIM900A_INTERRUPT_EVENT);
        }		
				res = USART_ReceiveData( UART4 );  //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR
  }
}


//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void UART4_Init(u32 bound)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	// GPIOC时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);

 	//USART_DeInit(UART4);  //复位串口2
	
		 //USART2_TX   PC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PA2
   
    //USART2_RX	  PC.11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PA3
	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4,ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4,DISABLE);

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(UART4, &USART_InitStructure); //初始化串口	2  	  
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	/* 使能串口2接收中断 */
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	/* 使能串口2总线空闲中断 */
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
	USART_Cmd(UART4, ENABLE);                    //使能串口 	
	
}

//RS485发送len个字节
//buf 发送区首地址
//len 发送的字节数
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
	*len = 0;//默认为0
	delay_ms(10);//等待10ms，连续10ms没有接收到数据，则认为接收结束
	if(rxlen == USART4_RX_CNT&& rxlen)//接收到数据且接收完了
	{
		for(i=0; i<rxlen; i++)
		{
			buf[i] = USART4_RX_BUF[i];
		}
		*len = USART4_RX_CNT;//记录本次数据长度
		USART4_RX_CNT=0;//清零
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
							//发送到APP层 与服务器进行数据通信
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

