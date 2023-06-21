//时间：2023/6/19 版本B
//作者：朱浩然 学号：21221289
//本程序内含我的个人信息，建议不要直接使用，找我把里面的信息删完再用，一定来找我，不然自己删不干净
//如有不懂请看注释
//部分文件内有我打的注释，把C文件都看看
//74HC595没有我打的注释，OLED的文件我没打注释
//，这两个文件的函数自己要看懂，24cxx和w25qxx是多余的文件，有能力就自己删，没能力建议就不要动了，不影响
//建议提前看看，不懂来问我


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

int main(void)//案例演示2022年嵌入式期末考核，主函数
{	 
	//系统初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();//延时初始化，此函数必须首个初始化
	uart_init(115200);//串口初始化，波特率115200可更改
	IIC_Init();//iic通讯初始化，此函数必须放在OLED初始化之前
	SPI2_Init();//SPI通讯初始化
	EXTIX_Init();//中断初始化，若要使用KEY_Scan()函数，则该函数需要删除
	TIM2_Init(1000-1,7200-1,0,3);//TIM2时钟中断初始化，分别是重装载，预分频，占线优先级，子优先级
	PWM4CH3_Init(1000-1,72000-1,TIM_OCMode_PWM1);//TIM4CH3的PWM初始化，分别是重装载，预分频，PWM模式（1为占高电平，2为占低电平），默认占空比50%
	//TIM与PWM有多个函数可以初始化调用，按实际情况来，具体看pwm.c和timer.c
	Dac1_Init();//数模转化初始化，输出口PA4
	Adc_Init();//模数转换初始化，输入口PA5
	//Photoresistor_Init();//光敏电阻检测初始化，不能与ADC初始化同时存在，使用时请删除Adc_Init()
	Init74HC595();//数码管初始化
	LED_Init();	//KED灯初始化，默认3个灯开启
	KEY_Init();//按键初始化
	OLED_init();//OLED屏幕初始化
	//W25QXX_Init();
	//AT24CXX_Init();	
	
	//以下语句用于其他必要初始化
	DAC_SetChannel1Data(DAC_Align_12b_R,0);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
	OLED_clear();
	
	
	
	OLED_ShowString(0,0,"21221289",16);//显示学号
	
	while(1)
	{	
		lightADC = Get_Average(u16ADCConvertedValue,16);
		light=100-((float)lightADC/4096)*100;
		//获取光强值
		
		Light_Show();//OLED显示光强电压等
		
		TIM4_SetAutoreload(ard);
		memset(temp,0,sizeof(temp));
		sprintf(temp,"%4dms,%2d",ard,50);	
		OLED_ShowString(0,16,temp,16);
		//更改，显示周期。显示占空比
	}
	
}

void keyfunction()//按键功能编辑（与中断冲突，主函数中使用此功能必须关掉外部中断，最简单的方法就是删掉EXTIX_Init()）
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
void ADC_Show()//显示ADC采集电压值（与显示光强冲突）,显示格式：ADC: 1. 78V
	{
	u16 u16ScanCnt,u16gtmp,adcx,adcy;
	char buff[64];
	float temp;

	if(u16ADCOK == 0x55D5)
		{			
			LED1=!LED1;
			u16gtmp = Get_Average(u16ADCConvertedValue,16);
			temp=(float)u16gtmp*(3.3/4096);	//获取计算后的带小数的实际电压值，比如3.1111。如3.1111，就显示3
			adcx=temp;                   		//赋值整数部分给adcx变量，因为adcx为u16整型
			temp-=adcx;                   	//把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111
			temp*=1000;                     //小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。
			adcy = temp;
			
			memset(buff,0,sizeof(buff));
			sprintf(buff,"ADC:%2d.%.3dV",adcx,adcy);	
			OLED_ShowString(0,0,buff,16);
			OLED_Refresh_Gram();
			u16ADCOK = 0x0000;  //ADC未转换	
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);							
		}
		if(u16ScanCnt%60==0x00)	LED1 = !LED1;	
}

void Light_Show()//显示光强值（与显示电压冲突），显示格式：Vol: 365mV,34
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
void TIM2_IRQHandler(void)//TIM2定时器中断服务函数，TIM2定时运行的函数放到这个里面的//code处，以下同理
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
		//code
	}
}

void TIM4_IRQHandler(void)//TIM4定时器中断服务函数
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );
		//code
	}
}
void EXTI0_IRQHandler(void)//外部中断0服务程序KEY3按下执行的函数放进这个的//code处，以下同理
{
	delay_ms(10);	//消抖
	if(KEY3==0)	 
	{
	//code
		ard+=100;
		if(ard>1500) ard=1500;
		//LED3=!LED3;
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位 
}
void EXTI1_IRQHandler(void)//外部中断1服务程序KEY2按下
{
	delay_ms(10);	//消抖
	if(KEY2==0)	 
	{				 
	//code
		ard-=100;
		if(ard<500) ard=500;
		//LED3=!LED3;
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line1);//清除LINE1上的中断标志位  
}
void EXTI2_IRQHandler(void)//外部中断2服务程序KEY1按下
{
	delay_ms(10);	//消抖
	if(KEY1==0)	  
	{				 
	//code 
		LED3=!LED3;
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line2);//清除LINE2上的中断标志位 
}
void EXTI3_IRQHandler(void)//外部中断3服务程序KEY4按下
{
	delay_ms(10);	//消抖
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
		//发送数据帧，学号8位+光强1位，均为16进制发送，注意串口助手设置16进制接受
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line3);  //清除LINE3上的中断标志位  
}
//