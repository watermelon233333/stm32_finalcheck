#ifndef __PWM_H
#define __PWM_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 								  
//////////////////////////////////////////////////////////////////////////////////   

void PWM4CH3_Init(u32 arr,u32 psc,u16 mode);//LED3
void PWM4CH4_Init(u32 arr,u32 psc,u16 mode);
void PWM2CH2_Init(u32 arr,u32 psc,u16 mode);//LED2
void TIM2_Setcompare2(u16 compare);
void TIM4_Setcompare3(u16 compare);
void TIM4_Setcompare4(u16 compare);
void TIM2_SetAutoreload(u16 arr);
void TIM4_SetAutoreload(u16 arr);
#endif