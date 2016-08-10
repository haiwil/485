#ifndef __MALLOC_H
#define __MALLOC_H
#include "stm32f10x.h"
 
#ifndef NULL
#define NULL 0
#endif

//定义三个内存池
#define SRAMIN1	 0		//内部内存池
#define SRAMIN2  1		//内部内存池

#define SRAMBANK 	2	//定义支持的SRAM块数.	

//内存管理控制器
struct _m_mallco_dev
{
	void (*init)(u8);					//初始化
	u8 (*perused)(u8);		  	    	//内存使用率
	u8 	*membase[SRAMBANK];				//内存池 管理SRAMBANK个区域的内存
	u16 *memmap[SRAMBANK]; 				//内存管理状态表
	u8  memrdy[SRAMBANK]; 				//内存管理是否就绪
};
extern struct _m_mallco_dev mallco_dev;	 //在mallco.c里面定义

void mymemset(void *s,u8 c,u32 count);	//设置内存
void mymemcpy(void *des,void *src,u32 n);//复制内存   
void *myrevmemcpy( void *dst, const void *src, unsigned int len );
void my_mem_init(u8 memx);				//内存管理初始化函数(外/内部调用)
u32  my_mem_malloc(u8 memx,u32 size);	//内存分配(内部调用)
u8   my_mem_free(u8 memx,u32 offset);		//内存释放(内部调用)
u8   my_mem_perused(u8 memx);				//获得内存使用率(外/内部调用) 
void myfree(u8 memx,void *ptr);  			//内存释放
void *mymalloc(u8 memx,u32 size);			//内存分配
void *myrealloc(u8 memx,void *ptr,u32 size);//重新分配内存

/*......................................................................*/
//用户调用函数 (外部调用) 

//消息链表，临时存包链表
void LinkedListMemInit(void);
u8   LinkedListMemPerused(void);
void LinkedListMemFree(void *ptr);
void *LinkedListMalloc(u32 size);

//定时器事件链表专用
void TimerEventListMemInit(void);
u8   TimerEventListMemPerused(void);
void TimerEventListMemFree(void *ptr);
void *TimerEventListMalloc(u32 size);


#endif













