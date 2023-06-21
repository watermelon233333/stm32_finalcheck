#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

//LED端口定义
#define LED1 PAout(1)	// DS0
#define LED2 PEout(6)	// DS1	 
#define LED3 PBout(8)	// DS0

void LED_Init(void);//初始化
	 				    
#endif
