#ifndef __STMFLASH_H__
#define __STMFLASH_H__

#include "sys.h"  
#include "types.h"

//读取ChipID
#define MCU_ID0   (*(vu32*)(0x1FFF7A10))
#define MCU_ID1   (*(vu32*)(0x1FFF7A14))
#define MCU_ID2   (*(vu32*)(0x1FFF7A18))

#define STM32_FLASH_SIZE 	64 	 			//所选STM32的FLASH容量大小(单位为K)

//FLASH解锁键值
#define FLASH_KEY1               0X45670123
#define FLASH_KEY2               0XCDEF89AB

#define STM_SECTOR_SIZE             1024 //字节
/*MY_FLASH_TABLE_WORD_NUMS 必需大于 MY_FLASH_PAGE_NUMS*/
#define MY_FLASH_BLOCK_BASE        (FLASH_BASE + 50*1024)
#define MY_FLASH_BLOCK_SIZE        (10*1024)
#define MY_FLASH_PAGE_SIZE         (128)

#define MY_FLASH_TABLE_BASE         (MY_FLASH_BLOCK_BASE - STM_SECTOR_SIZE)
#define MY_FLASH_TABLE_SIZE          STM_SECTOR_SIZE
#define MY_FLASH_TABLE_HALF_WORD_NUMS    (MY_FLASH_TABLE_SIZE/2)

#define MY_FLASH_PAGE_BASE(x)  (MY_FLASH_BLOCK_BASE + MY_FLASH_PAGE_SIZE*x)
#define MY_FLASH_PAGE_NUMS     (MY_FLASH_BLOCK_SIZE/MY_FLASH_PAGE_SIZE)

/*修改此结构体增加字段信息，最好对齐结构体字段，节约内存*/
typedef struct 
{ 
	//  mac 
    u32  MACAddrInFlash;
    u8 UartBoudInFlash;	
}FLASH_StoredInfoStru_t;

#define STORED_INFO_BYTE_SIZE     sizeof(FLASH_StoredInfoStru_t)	 
#define STORED_INFO_HALF_WORD_SIZE  (STORED_INFO_BYTE_SIZE/2+((STORED_INFO_BYTE_SIZE%2)?1:0))

typedef union  
{
        u16                          HalfWordBuf[STORED_INFO_HALF_WORD_SIZE];
		FLASH_StoredInfoStru_t       struInfo;  
}FLASH_StoredInfoUnion_t;

void STMFLASH_Unlock(void);					  	//FLASH解锁
void STMFLASH_Lock(void);					  	//FLASH上锁
u8 STMFLASH_GetStatus(void);				  	//获得状态
u8 STMFLASH_WaitDone(u16 time);				  	//等待操作结束
u8 STMFLASH_ErasePage(u32 paddr);			  	//擦除页
u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat);	//写入半字
u16 STMFLASH_ReadHalfWord(u32 faddr);		  	//读出半字  

int32 STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//从指定地址开始读出指定长度的数据
int32 MyFlashPageErase(u32 addr);
void MyFlashBlockErase(void);

/*FlashStoInfoUion操作方式: read --> modify --> store*/
extern FLASH_StoredInfoUnion_t FlashStoInfoUion;
/*保存全局变量 FlashStoInfoUion的信息*/
extern ErrorStatus StoredInfoToFlash(void);
/*读取信息到全局变量 FlashStoInfoUion*/
extern ErrorStatus ReadInfoFromFlash(void);

#endif

















