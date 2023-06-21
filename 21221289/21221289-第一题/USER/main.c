//ʱ�䣺2023/6/19 �汾B
//���ߣ����Ȼ ѧ�ţ�21221289


#include "sys.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "74HC595_LED.h"
#include "24cxx.h"
#include "adc.h"
#include "dac.h"
#include "spi.h"
#include "OLED4PINiic.h"
#include "w25qxx.h"
#include "spi.h"
#include "rhine.h"

void ADC_Show(void);
void Light_Show(void);
void keyfunction(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);

u8 u8DispBuf[5],value[5];
u16 u16ADCConvertedValue[64],u16ADCOK=0,u16DacVal;
u16 lightADC,light;

u16 ard=1000;
char temp[64];
u16 time=1;

u8 buff[4];

int main(void)
{	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	uart_init(115200);
	IIC_Init();
	SPI2_Init();
	EXTIX_Init();
	TIM3_Init(10000-1,7200-1,0,3);
	TIM4_Init(10-1,720-1,0,3);
	PWM4CH3_Init(1000-1,72000-1,TIM_OCMode_PWM1);
	Dac1_Init();
	Adc_Init();
	//Photoresistor_Init();
	Init74HC595();
	LED_Init();	
	KEY_Init();
	Init74HC595();
	OLED_init();
	//W25QXX_Init();
	//AT24CXX_Init();	
	DAC_SetChannel1Data(DAC_Align_12b_R,0);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
	OLED_clear();
	
	OLED_ShowString(0,48,"ZHR-21221289",12);//��ʾѧ��
	OLED_Refresh_Gram();
	LED1=0;
	LED2=0;
	LED3=0;
	while(1)
	{	
		
		DispUpdate1(buff[0],buff[1],buff[2],buff[3]);
		LED3=!((time+1)%3);
		LED1=!((time+2)%3);
		LED2=!((time+3)%3);
	}
	
}

void keyfunction()
{
u8 keyscan;
keyscan=KEY_Scan(1);
		switch(keyscan){
			case 1: 
				/*code*/;//KEY1
			break;
			case 2: 
				/*code*/;//KEY2
			break;
			case 3: 
				/*code*/;//KEY3
			break;
			case 4: 
				/*code*/;//KEY4
			break;
		}
}
void ADC_Show()//��ʾADC�ɼ���ѹֵ
	{
	u16 u16ScanCnt,u16gtmp,adcx,adcy;
	char buff[64];
	float temp;

	if(u16ADCOK == 0x55D5)
		{			
			LED1=!LED1;
			u16gtmp = Get_Average(u16ADCConvertedValue,16);
			temp=(float)u16gtmp*(3.3/4096);	//��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111����3.1111������ʾ3
			adcx=temp;                   		//��ֵ�������ָ�adcx��������ΪadcxΪu16����
			temp-=adcx;                   	//���Ѿ���ʾ����������ȥ��������С�����֣�����3.1111-3=0.1111
			temp*=1000;                     //С�����ֳ���1000�����磺0.1111��ת��Ϊ111.1���൱�ڱ�����λС����
			adcy = temp;
			
			memset(buff,0,sizeof(buff));
			sprintf(buff,"ADC:%2d.%.3dV",adcx,adcy);	
			OLED_ShowString(0,0,buff,16);
			OLED_Refresh_Gram();
			u16ADCOK = 0x0000;  //ADCδת��	
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);							
		}
		if(u16ScanCnt%60==0x00)	LED1 = !LED1;	
}

void Light_Show()//��ʾ��ǿֵ
	{
	char buff[64];	
	u16 u16ScanCnt,u16gtmp,val,vol,adcx,adcy;;

	if(u16ADCOK == 0x55D5)
		{			
			LED1=!LED1;
			u16gtmp = Get_Average(u16ADCConvertedValue,16);
			val=((float)u16gtmp/4096)*100;
			vol=((float)u16gtmp/4096)*3300;
			
			memset(buff,0,sizeof(buff));
			sprintf(buff,"Vol:%4dmV,%3d",vol,100-val);	
			OLED_ShowString(0,32,buff,12);
			OLED_Refresh_Gram();
			u16ADCOK = 0x0000;
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);							
		}
		if(u16ScanCnt%60==0x00)	LED1 = !LED1;	
}
void TIM2_IRQHandler(void)//TIM2��ʱ���жϷ�����
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );
		//code
	}
}

void TIM3_IRQHandler(void)//TIM3��ʱ���жϷ�����
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );
		time++;
		buff[0]=(time/1000)%10;
		buff[1]=(time/100)%10;
		buff[2]=(time/10)%10;
		buff[3]=(time/1)%10;
	}
}

void TIM4_IRQHandler(void)//TIM4��ʱ���жϷ�����
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );
	}
}
void EXTI0_IRQHandler(void)//�ⲿ�ж�0�������KEY3����
{
	delay_ms(10);	//����
	if(KEY3==0)	 
	{
	//code
		ard+=100;
		if(ard>1500) ard=1500;
		//LED3=!LED3;
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ 
}
void EXTI1_IRQHandler(void)//�ⲿ�ж�1�������KEY2����
{
	delay_ms(10);	//����
	if(KEY2==0)	 
	{				 
	//code
		ard-=100;
		if(ard<500) ard=500;
		//LED3=!LED3;
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line1);//���LINE1�ϵ��жϱ�־λ  
}
void EXTI2_IRQHandler(void)//�ⲿ�ж�2�������KEY1����
{
	delay_ms(10);	//����
	if(KEY1==0)	  
	{				 
	//code 
		LED3=!LED3;
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line2);//���LINE2�ϵ��жϱ�־λ 
}
void EXTI3_IRQHandler(void)//�ⲿ�ж�3�������KEY4����
{
	delay_ms(10);	//����
	if(KEY4==0)	 
	{
		//code
		USART_SendData(USART1,2);
		delay_ms(5);
		USART_SendData(USART1,1);
		delay_ms(5);
		USART_SendData(USART1,2);
		delay_ms(5);
		USART_SendData(USART1,2);
		delay_ms(5);
		USART_SendData(USART1,1);
		delay_ms(5);
		USART_SendData(USART1,2);
		delay_ms(5);
		USART_SendData(USART1,8);
		delay_ms(5);
		USART_SendData(USART1,9);
		delay_ms(5);
		USART_SendData(USART1,light);
		delay_ms(5);
		//��������֡��ѧ��8λ+��ǿ1λ����Ϊ16���Ʒ��ͣ�ע�⴮����������16���ƽ���
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line3);  //���LINE3�ϵ��жϱ�־λ  
}
//