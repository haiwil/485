
#ifndef __DELAY_H
#define __DELAY_H 			   
#include <sys.h>	  

#define OS_TICKS_PER_SEC	1000 //OSʱ�ӽ���,��ÿ����ȴ���
#define time_after(a,b)      ((s32)(b) - (s32)(a) <= 0) //�ж�b�Ƿ���a֮��

extern vu32 e_systick_val;

void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif





























