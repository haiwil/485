#ifndef __MALLOC_H
#define __MALLOC_H
#include "stm32f10x.h"
 
#ifndef NULL
#define NULL 0
#endif

//���������ڴ��
#define SRAMIN1	 0		//�ڲ��ڴ��
#define SRAMIN2  1		//�ڲ��ڴ��

#define SRAMBANK 	2	//����֧�ֵ�SRAM����.	

//�ڴ����������
struct _m_mallco_dev
{
	void (*init)(u8);					//��ʼ��
	u8 (*perused)(u8);		  	    	//�ڴ�ʹ����
	u8 	*membase[SRAMBANK];				//�ڴ�� ����SRAMBANK��������ڴ�
	u16 *memmap[SRAMBANK]; 				//�ڴ����״̬��
	u8  memrdy[SRAMBANK]; 				//�ڴ�����Ƿ����
};
extern struct _m_mallco_dev mallco_dev;	 //��mallco.c���涨��

void mymemset(void *s,u8 c,u32 count);	//�����ڴ�
void mymemcpy(void *des,void *src,u32 n);//�����ڴ�   
void *myrevmemcpy( void *dst, const void *src, unsigned int len );
void my_mem_init(u8 memx);				//�ڴ������ʼ������(��/�ڲ�����)
u32  my_mem_malloc(u8 memx,u32 size);	//�ڴ����(�ڲ�����)
u8   my_mem_free(u8 memx,u32 offset);		//�ڴ��ͷ�(�ڲ�����)
u8   my_mem_perused(u8 memx);				//����ڴ�ʹ����(��/�ڲ�����) 
void myfree(u8 memx,void *ptr);  			//�ڴ��ͷ�
void *mymalloc(u8 memx,u32 size);			//�ڴ����
void *myrealloc(u8 memx,void *ptr,u32 size);//���·����ڴ�

/*......................................................................*/
//�û����ú��� (�ⲿ����) 

//��Ϣ��������ʱ�������
void LinkedListMemInit(void);
u8   LinkedListMemPerused(void);
void LinkedListMemFree(void *ptr);
void *LinkedListMalloc(u32 size);

//��ʱ���¼�����ר��
void TimerEventListMemInit(void);
u8   TimerEventListMemPerused(void);
void TimerEventListMemFree(void *ptr);
void *TimerEventListMalloc(u32 size);


#endif












