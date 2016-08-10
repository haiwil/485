#ifndef __STMFLASH_H__
#define __STMFLASH_H__

#include "sys.h"  
#include "types.h"

//��ȡChipID
#define MCU_ID0   (*(vu32*)(0x1FFF7A10))
#define MCU_ID1   (*(vu32*)(0x1FFF7A14))
#define MCU_ID2   (*(vu32*)(0x1FFF7A18))

#define STM32_FLASH_SIZE 	64 	 			//��ѡSTM32��FLASH������С(��λΪK)

//FLASH������ֵ
#define FLASH_KEY1               0X45670123
#define FLASH_KEY2               0XCDEF89AB

#define STM_SECTOR_SIZE             1024 //�ֽ�
/*MY_FLASH_TABLE_WORD_NUMS ������� MY_FLASH_PAGE_NUMS*/
#define MY_FLASH_BLOCK_BASE        (FLASH_BASE + 50*1024)
#define MY_FLASH_BLOCK_SIZE        (10*1024)
#define MY_FLASH_PAGE_SIZE         (128)

#define MY_FLASH_TABLE_BASE         (MY_FLASH_BLOCK_BASE - STM_SECTOR_SIZE)
#define MY_FLASH_TABLE_SIZE          STM_SECTOR_SIZE
#define MY_FLASH_TABLE_HALF_WORD_NUMS    (MY_FLASH_TABLE_SIZE/2)

#define MY_FLASH_PAGE_BASE(x)  (MY_FLASH_BLOCK_BASE + MY_FLASH_PAGE_SIZE*x)
#define MY_FLASH_PAGE_NUMS     (MY_FLASH_BLOCK_SIZE/MY_FLASH_PAGE_SIZE)

/*�޸Ĵ˽ṹ�������ֶ���Ϣ����ö���ṹ���ֶΣ���Լ�ڴ�*/
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

void STMFLASH_Unlock(void);					  	//FLASH����
void STMFLASH_Lock(void);					  	//FLASH����
u8 STMFLASH_GetStatus(void);				  	//���״̬
u8 STMFLASH_WaitDone(u16 time);				  	//�ȴ���������
u8 STMFLASH_ErasePage(u32 paddr);			  	//����ҳ
u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat);	//д�����
u16 STMFLASH_ReadHalfWord(u32 faddr);		  	//��������  

int32 STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
int32 MyFlashPageErase(u32 addr);
void MyFlashBlockErase(void);

/*FlashStoInfoUion������ʽ: read --> modify --> store*/
extern FLASH_StoredInfoUnion_t FlashStoInfoUion;
/*����ȫ�ֱ��� FlashStoInfoUion����Ϣ*/
extern ErrorStatus StoredInfoToFlash(void);
/*��ȡ��Ϣ��ȫ�ֱ��� FlashStoInfoUion*/
extern ErrorStatus ReadInfoFromFlash(void);

#endif

















