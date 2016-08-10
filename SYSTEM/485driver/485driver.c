
#include "main.h"

#ifdef EN_USART2_RX //如果使能了接收
//接收缓存区
u8 RS485_RX_BUF[USART2_MAX_RECV_LEN]={0};
//接收到的数据长度
u8 RS485_RX_CNT=0;

u8 USART2_Send_String(char* string);
void USART2_Send_Data(u8* data, u16 len);

void USART2_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)//接收数据
	{
		res = USART_ReceiveData(USART2);//读取接收数据
		//Debug_printf("%c",res);
		if(RS485_RX_CNT<USART2_MAX_RECV_LEN)
		{
				RS485_RX_BUF[RS485_RX_CNT] = res;
				RS485_RX_CNT++;
		}
		else//接收错误
		{
			RS485_RX_CNT = 0;
		}
	}
	
	if ( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )      //数据帧接收完毕
	{
        if((RS485_RX_CNT&0x8000)==0)//接收未完成
				{
            RS485_RX_CNT|=0x8000;	//接收完成了
						osal_SetInterruptEvent(SJ485_INTERRUPT_EVENT);
        }		
				res = USART_ReceiveData( USART2 );  //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR
  }
}
#endif

//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void RS485_Init(u32 bound)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//USART2_TX   PA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2
   
    //USART2_RX	  PA.3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3
	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//复位串口2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//停止复位
	
	#ifdef EN_USART2_RX
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//字长为9位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_Even;//偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口	2  	  
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	/* 使能串口2接收中断 */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	/* 使能串口2总线空闲中断 */
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	USART_Cmd(USART2, ENABLE);                    //使能串口 	
	#endif
	
	#ifdef Uart_Debug
	//RS485_Send_Data("init 485 ok!!!\n",15);
	#endif
}

//RS485发送len个字节
//buf 发送区首地址
//len 发送的字节数
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
	*len = 0;//默认为0
	delay_ms(10);//等待10ms，连续10ms没有接收到数据，则认为接收结束
	if(rxlen == RS485_RX_CNT&& rxlen)//接收到数据且接收完了
	{
		for(i=0; i<rxlen; i++)
		{
			buf[i] = RS485_RX_BUF[i];
		}
		*len = RS485_RX_CNT;//记录本次数据长度
		RS485_RX_CNT=0;//清零
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
  SIM900A_send_fifo.in = SIM900A_send_fifo.out = 0;//清FIFO
	RS485_Init(9600);
	
	osal_update_timerEx( 100, Hal_TaskID, HAL_LED_BLINK_EVENT);
	osal_update_timerEx( 100, App_TaskID, APP_SIM900A_HEART_BEAT_EVENT);
}

