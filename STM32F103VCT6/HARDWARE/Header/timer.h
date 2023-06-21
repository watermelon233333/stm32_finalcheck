#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 								  
//////////////////////////////////////////////////////////////////////////////////   

void TIM2_Init(u16 arr,u16 psc,u8 prePriority,u8 SubPriority);
void TIM3_Init(u16 arr,u16 psc,u8 prePriority,u8 SubPriority);
void TIM4_Init(u16 arr,u16 psc,u8 prePriority,u8 SubPriority);

#endif
