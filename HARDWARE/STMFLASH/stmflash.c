#include "stmflash.h"
#include "delay.h"
#include "usart.h"

FLASH_StoredInfoUnion_t FlashStoInfoUion;

//����STM32��FLASH
void STMFLASH_Unlock(void)
{
  FLASH->KEYR=FLASH_KEY1;//д���������.
  FLASH->KEYR=FLASH_KEY2;
}
//flash����
void STMFLASH_Lock(void)
{
  FLASH->CR|=1<<7;//����
}
//�õ�FLASH״̬
u8 STMFLASH_GetStatus(void)
{	
	u32 res;		
	res=FLASH->SR; 
	if(res&(1<<0))return 1;		    //æ
	else if(res&(1<<2))return 2;	//��̴���
	else if(res&(1<<4))return 3;	//д��������
	return 0;						//�������
}
//�ȴ��������
//time:Ҫ��ʱ�ĳ���
//����ֵ:״̬.
u8 STMFLASH_WaitDone(u16 time)
{
	u8 res;
	do
	{
		res=STMFLASH_GetStatus();
		if(res!=1)break;//��æ,����ȴ���,ֱ���˳�.
		delay_us(1);
		time--;
	 }while(time);
	 if(time==0)res=0xff;//TIMEOUT
	 return res;
}
//����ҳ
//paddr:ҳ��ַ
//����ֵ:ִ�����
u8 STMFLASH_ErasePage(u32 paddr)
{
	u8 res=0;
	res=STMFLASH_WaitDone(0X5FFF);//�ȴ��ϴβ�������,>20ms    
	if(res==0)
	{ 
		FLASH->CR|=1<<1;//ҳ����
		FLASH->AR=paddr;//����ҳ��ַ 
		FLASH->CR|=1<<6;//��ʼ����		  
		res=STMFLASH_WaitDone(0X5FFF);//�ȴ���������,>20ms  
		if(res!=1)//��æ
		{
			FLASH->CR&=~(1<<1);//���ҳ������־.
		}
	}
	return res;
}
//��FLASHָ����ַд�����
//faddr:ָ����ַ(�˵�ַ����Ϊ2�ı���!!)
//dat:Ҫд�������
//����ֵ:д������
u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat)
{
	u8 res;	   	    
	res=STMFLASH_WaitDone(0XFF);	 
	if(res==0)//OK
	{
		FLASH->CR|=1<<0;//���ʹ��
		*(vu16*)faddr=dat;//д������
		res=STMFLASH_WaitDone(0XFF);//�ȴ��������
		if(res!=1)//�����ɹ�
		{
			FLASH->CR&=~(1<<0);//���PGλ.
		}
	} 
	return res;
} 
//��ȡָ����ַ�İ���(16λ����) 
//faddr:����ַ 
//����ֵ:��Ӧ����.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}

//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
    STMFLASH_Unlock();//���� 
	for(i=0;i<NumToWrite;i++)
	{
		STMFLASH_WriteHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//��ַ����2.
	}  
    STMFLASH_Lock();//����
} 
//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
//pBuffer:����ָ��
//NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
int32 STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)
{ 
    int32 status = 0;
	u32 addrx=0;
	u32 endaddr=0;	
    
    if(WriteAddr<FLASH_BASE||WriteAddr%2)
        return -2;	//�Ƿ���ַ     
	addrx=WriteAddr;				//д�����ʼ��ַ
	endaddr=WriteAddr+NumToWrite*2;	//д��Ľ�����ַ
    while(addrx<endaddr)		//ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
	{
		if( STMFLASH_ReadHalfWord(addrx) != 0XFFFF )//�з�0XFFFFFFFF�ĵط�,Ҫ�����������
		{   
			status=MyFlashPageErase(addrx);
			if(status!=0) 
               break;	//����������
		}else addrx+=2;
	} 
	if(status==0)
	{    
        STMFLASH_Write_NoCheck(WriteAddr,pBuffer,NumToWrite);
	}
    return status;
} 

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}


//����addr��Ӧ��flash����
//addr:flash��ַ
//����ֵ: 0 = pass
int32 MyFlashPageErase(u32 addr)
{
   int32 stat;
   if(addr<FLASH_BASE||addr%2)//�Ƿ���ַ
        return FLASH_ERROR_PG;	
   STMFLASH_Unlock();//����
   //ȡaddr��Ӧ��flash�����׵�ַ
   addr = addr & (~(STM_SECTOR_SIZE - 1)); 
   stat = STMFLASH_ErasePage(addr);
   STMFLASH_Lock();//����
   return stat;
}


//�����Զ����Block��������ҳ��
void MyFlashBlockErase(void)
{
   u32 i;
   STMFLASH_Unlock();//����
   STMFLASH_ErasePage(MY_FLASH_TABLE_BASE);//�����������
   for(i=0; i<(MY_FLASH_BLOCK_SIZE/STM_SECTOR_SIZE); i++)
     STMFLASH_ErasePage(MY_FLASH_BLOCK_BASE+STM_SECTOR_SIZE*i);
   STMFLASH_Lock();//����
}

/*����ȫ�ֱ��� FlashStoInfoUion����Ϣ*/
ErrorStatus StoredInfoToFlash(void)
{
  u16 page_index;
  u32 pagex_base;
  u32 table_addr;

  //ҳ������������С��ҳ��
  if( MY_FLASH_TABLE_HALF_WORD_NUMS < MY_FLASH_PAGE_NUMS ) return ERROR;
  	
  table_addr = MY_FLASH_TABLE_BASE;
  //��λ����һ����ҳ��
  for(page_index=0; page_index < MY_FLASH_PAGE_NUMS; page_index++)
  {
  	if( STMFLASH_ReadHalfWord(table_addr) == 0XFFFF ) break;
  	table_addr += 2; //ƫ��2���ֽ�.	
  }
  if(page_index >= MY_FLASH_PAGE_NUMS){
	page_index = 0;  
  } 
  if(page_index == 0){ 
    if(MyFlashPageErase( MY_FLASH_TABLE_BASE ) != 0) 
        return ERROR;
  }
  //��ȡ��ǰҳ�Ļ���ַ
  pagex_base = MY_FLASH_PAGE_BASE(page_index);
  //��ǰҳ��д���Ӧҳ��
  STMFLASH_Write(MY_FLASH_TABLE_BASE + page_index*2, &page_index, 1);  
  //������д�뵱ǰҳ
  STMFLASH_Write(pagex_base, FlashStoInfoUion.HalfWordBuf, STORED_INFO_HALF_WORD_SIZE);
  return SUCCESS;
}

/*��ȡ��Ϣ��ȫ�ֱ��� FlashStoInfoUion*/
ErrorStatus ReadInfoFromFlash(void)
{
  u16 page_index;
  u32 pagex_base;
  u32 table_addr;
  //ҳ������������С��ҳ��
  if( MY_FLASH_TABLE_HALF_WORD_NUMS < MY_FLASH_PAGE_NUMS ) return ERROR;
  	
  table_addr = MY_FLASH_TABLE_BASE;
  //��λ����һ����ҳ��
  for(page_index=0; page_index < MY_FLASH_PAGE_NUMS; page_index++)
  {
  	if( STMFLASH_ReadHalfWord(table_addr) == 0XFFFF ) break;
  	table_addr += 2; //ƫ��2���ֽ�.	
  }
  //��λ���һ���ǿ�ҳ��
  if(page_index>0) page_index--; 
  //��ȡ��ǰҳ�Ļ���ַ
  pagex_base = MY_FLASH_PAGE_BASE(page_index);  
  //��ȡ��ǰҳ�����ݵ�������
  STMFLASH_Read(pagex_base, FlashStoInfoUion.HalfWordBuf, STORED_INFO_HALF_WORD_SIZE);
  return SUCCESS;
}


