
#ifndef __DELAY_H
#define __DELAY_H 			   
#include <sys.h>	  

#define OS_TICKS_PER_SEC	1000 //OS时钟节拍,即每秒调度次数
#define time_after(a,b)      ((s32)(b) - (s32)(a) <= 0) //判断b是否在a之后

extern vu32 e_systick_val;

void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif





























