//时间：2023/6/21 版本1
//作者：朱浩然 学号：21221289


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

u16 val;
u16 temp;
u16 va=2000;

u8 buff[50];

int main(void)
{	 
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	uart_init(115200);
	IIC_Init();
	SPI2_Init();
	EXTIX_Init();
	Dac1_Init();
	Adc_Init();
	Init74HC595();
	LED_Init();	
	KEY_Init();
	OLED_init();
	DAC_SetChannel1Data(DAC_Align_12b_R,va);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
	
	OLED_clear();
	OLED_ShowString(0,48,"ZHR-21221289",12);
	OLED_Refresh_Gram();
	while(1)
	{	
		temp = Get_Average(u16ADCConvertedValue,16);
		val =((float)temp/4096)*3300;
		DAC_SetChannel1Data(DAC_Align_12b_R,va);
		
		printf("ADval:%4dmV",val);
		printf("\n\n");
		memset(buff,0,sizeof(buff));
		sprintf(buff,"ADval:%4dmV",val);
		OLED_ShowString(0,0,buff,16);
		OLED_Refresh_Gram();
	}
}

void TIM2_IRQHandler(void)//TIM2定时器中断服务函数
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );
		//code
	}
}

void TIM3_IRQHandler(void)//TIM3定时器中断服务函数
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );
	}
}

void TIM4_IRQHandler(void)//TIM4定时器中断服务函数
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );
	}
}
void EXTI0_IRQHandler(void)//外部中断0服务程序KEY3按下
{
	delay_ms(10);	//消抖
	if(KEY3==0)	 
	{
		va-=200;
		if(va<200) va=200; 
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位 
}
void EXTI1_IRQHandler(void)//外部中断1服务程序KEY2按下
{
	delay_ms(10);	//消抖
	if(KEY2==0)	 
	{
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line1);//清除LINE1上的中断标志位  
}
void EXTI2_IRQHandler(void)//外部中断2服务程序KEY1按下
{
	delay_ms(10);	//消抖
	if(KEY1==0)	  
	{			
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line2);//清除LINE2上的中断标志位 
}
void EXTI3_IRQHandler(void)//外部中断3服务程序KEY4按下
{
	delay_ms(10);	//消抖
	if(KEY4==0)	 
	{
		va+=200;
		if(va>4000) va=4000; 
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line3);  //清除LINE3上的中断标志位  
}
//