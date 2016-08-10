#include "stmflash.h"
#include "delay.h"
#include "usart.h"

FLASH_StoredInfoUnion_t FlashStoInfoUion;

//解锁STM32的FLASH
void STMFLASH_Unlock(void)
{
  FLASH->KEYR=FLASH_KEY1;//写入解锁序列.
  FLASH->KEYR=FLASH_KEY2;
}
//flash上锁
void STMFLASH_Lock(void)
{
  FLASH->CR|=1<<7;//上锁
}
//得到FLASH状态
u8 STMFLASH_GetStatus(void)
{	
	u32 res;		
	res=FLASH->SR; 
	if(res&(1<<0))return 1;		    //忙
	else if(res&(1<<2))return 2;	//编程错误
	else if(res&(1<<4))return 3;	//写保护错误
	return 0;						//操作完成
}
//等待操作完成
//time:要延时的长短
//返回值:状态.
u8 STMFLASH_WaitDone(u16 time)
{
	u8 res;
	do
	{
		res=STMFLASH_GetStatus();
		if(res!=1)break;//非忙,无需等待了,直接退出.
		delay_us(1);
		time--;
	 }while(time);
	 if(time==0)res=0xff;//TIMEOUT
	 return res;
}
//擦除页
//paddr:页地址
//返回值:执行情况
u8 STMFLASH_ErasePage(u32 paddr)
{
	u8 res=0;
	res=STMFLASH_WaitDone(0X5FFF);//等待上次操作结束,>20ms    
	if(res==0)
	{ 
		FLASH->CR|=1<<1;//页擦除
		FLASH->AR=paddr;//设置页地址 
		FLASH->CR|=1<<6;//开始擦除		  
		res=STMFLASH_WaitDone(0X5FFF);//等待操作结束,>20ms  
		if(res!=1)//非忙
		{
			FLASH->CR&=~(1<<1);//清除页擦除标志.
		}
	}
	return res;
}
//在FLASH指定地址写入半字
//faddr:指定地址(此地址必须为2的倍数!!)
//dat:要写入的数据
//返回值:写入的情况
u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat)
{
	u8 res;	   	    
	res=STMFLASH_WaitDone(0XFF);	 
	if(res==0)//OK
	{
		FLASH->CR|=1<<0;//编程使能
		*(vu16*)faddr=dat;//写入数据
		res=STMFLASH_WaitDone(0XFF);//等待操作完成
		if(res!=1)//操作成功
		{
			FLASH->CR&=~(1<<0);//清除PG位.
		}
	} 
	return res;
} 
//读取指定地址的半字(16位数据) 
//faddr:读地址 
//返回值:对应数据.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}

//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
    STMFLASH_Unlock();//解锁 
	for(i=0;i<NumToWrite;i++)
	{
		STMFLASH_WriteHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//地址增加2.
	}  
    STMFLASH_Lock();//上锁
} 
//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
int32 STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)
{ 
    int32 status = 0;
	u32 addrx=0;
	u32 endaddr=0;	
    
    if(WriteAddr<FLASH_BASE||WriteAddr%2)
        return -2;	//非法地址     
	addrx=WriteAddr;				//写入的起始地址
	endaddr=WriteAddr+NumToWrite*2;	//写入的结束地址
    while(addrx<endaddr)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
	{
		if( STMFLASH_ReadHalfWord(addrx) != 0XFFFF )//有非0XFFFFFFFF的地方,要擦除这个扇区
		{   
			status=MyFlashPageErase(addrx);
			if(status!=0) 
               break;	//发生错误了
		}else addrx+=2;
	} 
	if(status==0)
	{    
        STMFLASH_Write_NoCheck(WriteAddr,pBuffer,NumToWrite);
	}
    return status;
} 

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}


//擦除addr对应的flash扇区
//addr:flash地址
//返回值: 0 = pass
int32 MyFlashPageErase(u32 addr)
{
   int32 stat;
   if(addr<FLASH_BASE||addr%2)//非法地址
        return FLASH_ERROR_PG;	
   STMFLASH_Unlock();//解锁
   //取addr对应的flash扇区首地址
   addr = addr & (~(STM_SECTOR_SIZE - 1)); 
   stat = STMFLASH_ErasePage(addr);
   STMFLASH_Lock();//上锁
   return stat;
}


//擦除自定义的Block区，包括页表
void MyFlashBlockErase(void)
{
   u32 i;
   STMFLASH_Unlock();//解锁
   STMFLASH_ErasePage(MY_FLASH_TABLE_BASE);//擦除这个扇区
   for(i=0; i<(MY_FLASH_BLOCK_SIZE/STM_SECTOR_SIZE); i++)
     STMFLASH_ErasePage(MY_FLASH_BLOCK_BASE+STM_SECTOR_SIZE*i);
   STMFLASH_Lock();//上锁
}

/*保存全局变量 FlashStoInfoUion的信息*/
ErrorStatus StoredInfoToFlash(void)
{
  u16 page_index;
  u32 pagex_base;
  u32 table_addr;

  //页表数量不可以小于页数
  if( MY_FLASH_TABLE_HALF_WORD_NUMS < MY_FLASH_PAGE_NUMS ) return ERROR;
  	
  table_addr = MY_FLASH_TABLE_BASE;
  //定位到第一个空页表
  for(page_index=0; page_index < MY_FLASH_PAGE_NUMS; page_index++)
  {
  	if( STMFLASH_ReadHalfWord(table_addr) == 0XFFFF ) break;
  	table_addr += 2; //偏移2个字节.	
  }
  if(page_index >= MY_FLASH_PAGE_NUMS){
	page_index = 0;  
  } 
  if(page_index == 0){ 
    if(MyFlashPageErase( MY_FLASH_TABLE_BASE ) != 0) 
        return ERROR;
  }
  //获取当前页的基地址
  pagex_base = MY_FLASH_PAGE_BASE(page_index);
  //当前页数写入对应页表
  STMFLASH_Write(MY_FLASH_TABLE_BASE + page_index*2, &page_index, 1);  
  //把数据写入当前页
  STMFLASH_Write(pagex_base, FlashStoInfoUion.HalfWordBuf, STORED_INFO_HALF_WORD_SIZE);
  return SUCCESS;
}

/*读取信息到全局变量 FlashStoInfoUion*/
ErrorStatus ReadInfoFromFlash(void)
{
  u16 page_index;
  u32 pagex_base;
  u32 table_addr;
  //页表数量不可以小于页数
  if( MY_FLASH_TABLE_HALF_WORD_NUMS < MY_FLASH_PAGE_NUMS ) return ERROR;
  	
  table_addr = MY_FLASH_TABLE_BASE;
  //定位到第一个空页表
  for(page_index=0; page_index < MY_FLASH_PAGE_NUMS; page_index++)
  {
  	if( STMFLASH_ReadHalfWord(table_addr) == 0XFFFF ) break;
  	table_addr += 2; //偏移2个字节.	
  }
  //定位最后一个非空页表
  if(page_index>0) page_index--; 
  //获取当前页的基地址
  pagex_base = MY_FLASH_PAGE_BASE(page_index);  
  //读取当前页的数据到联合体
  STMFLASH_Read(pagex_base, FlashStoInfoUion.HalfWordBuf, STORED_INFO_HALF_WORD_SIZE);
  return SUCCESS;
}


