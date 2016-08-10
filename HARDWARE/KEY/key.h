#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

/*下面方式是通过位带操作方式读取IO*/
#define KEY0 		PAin(0)   	
#define KEY1 		PEin(3)		
#define KEY2 		PEin(2)		
	
#define KEY0_PRES 	1
#define KEY1_PRES	2
#define KEY2_PRES	3


void KEY_Init(void);	//IO初始化
u8 Get_KEY_Value(void);
void EXTIX_Init(void);


#endif
