#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

/*下面的方式是通过直接操作库函数方式读取IO*/
#define KEY1 	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2) //PC2
#define KEY2 	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)	//PC1 
#define KEY4 	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3) //PC3
#define KEY3 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//PA0


/*下面方式是通过位带操作方式读取IO*/
/*
#define KEY0 		PEin(4)   	//PE4
#define KEY1 		PEin(3)		//PE3 
#define KEY2 		PEin(2)		//P32
#define WK_UP 	PAin(0)		//PA0
*/

#define KEY1_PRES 	1
#define KEY2_PRES		2
#define KEY3_PRES		3	
#define KEY4_PRES   4

void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数					    
#endif
