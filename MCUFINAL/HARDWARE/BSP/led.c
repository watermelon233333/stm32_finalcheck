#include "led.h"
  

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);	 //使能PB,PE端口时钟
	
 //GPIOPA1初始化设置	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				  //LED1-->PA.1 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					    //根据设定参数初始化GPIOPA.1
 GPIO_SetBits(GPIOA,GPIO_Pin_1);						        //PA.1输出高

 //GPIOPB8初始化设置	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	    		//LED1-->PE.5 端口配置, 推挽输出
 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 	//推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOB,GPIO_Pin_8); 						 				//PE.5 输出高 
 
//GPIOPE6初始化设置	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	    		 //LED1-->PE.5 端口配置, 推挽输出
 GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOE,GPIO_Pin_6); 						 //PE.5 输出高 
}
 
