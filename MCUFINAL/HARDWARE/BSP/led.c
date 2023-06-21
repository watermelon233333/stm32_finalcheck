#include "led.h"
  

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
 //GPIOPA1��ʼ������	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				  //LED1-->PA.1 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					    //�����趨������ʼ��GPIOPA.1
 GPIO_SetBits(GPIOA,GPIO_Pin_1);						        //PA.1�����

 //GPIOPB8��ʼ������	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	    		//LED1-->PE.5 �˿�����, �������
 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 	//������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOB,GPIO_Pin_8); 						 				//PE.5 ����� 
 
//GPIOPE6��ʼ������	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	    		 //LED1-->PE.5 �˿�����, �������
 GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOE,GPIO_Pin_6); 						 //PE.5 ����� 
}
 
