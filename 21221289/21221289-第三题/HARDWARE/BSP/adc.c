#include "adc.h"
#include "delay.h"

#define ADC1_DR_Address				0x4001244C

extern uint16_t u16ADCConvertedValue[],u16ADCOK;
//////////////////////////////////////////////////////////////////////////////////	 								  
////////////////////////////////////////////////////////////////////////////////// 	
//初始化ADC															   
void  Adc_Init(void)
{    
	ADC_InitTypeDef		ADC_InitStructure; 
	GPIO_InitTypeDef	GPIO_InitStructure;
	DMA_InitTypeDef		DMA_InitStructure;//注：ADC为12位模数转换器，只有ADCConvertedValue的低12位有效
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);			//使能DMA时钟	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);	//使能ADC1通道时钟
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);		//ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//ADC1复位借宿

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);		//72M/6=12,ADC最大时间不能超过14M
	
  //初始化ADC1通道3、7 IO口 //ADC123_IN3//ADC12_IN7                        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	//PA3通道3、PA7通道7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;					//模拟输入引脚
  GPIO_Init(GPIOA, &GPIO_InitStructure);								//初始化  		
	
	ADC_DeInit(ADC1);  //将外设 ADC1 的全部寄存器重设为缺省值
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;//独立的转换模式ADC_DUALMOD［3:0］=0000;
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;//关闭扫描模式因为只有一个通道
	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;//使能连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv= ADC_ExternalTrigConv_None;//软件转换模式
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//对齐方式，ADC为12位中，右对齐方式ADC_ALIGN=0;
	ADC_InitStructure.ADC_NbrOfChannel=1;//开启通道数，1个ADC_SQR1［23:20］=0000;	//ADC_SQR1［23:20］设置通道数目的选择
	ADC_Init(ADC1,&ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,1,ADC_SampleTime_1Cycles5);

	//配置ADC1的DMA通道
	DMA_DeInit(DMA1_Channel1);//开启DMA1的第一通道
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//DMA对应的外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr =(uint32_t)&u16ADCConvertedValue;//内存存储基地址
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//DMA的转换模式为SRC模式，由外设搬移到内存
	DMA_InitStructure.DMA_BufferSize=16;//DMA缓存大小，1个
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//接收一次数据后，设备地址禁止后移
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//关闭接收一次数据后，目标内存地址后移 DMA_MemoryInc_Disable
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//定义外设数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//DMA搬移数据尺寸，HalfWord就是为16位
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;			//循环转换模式
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;	//DMA优先级高
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;				//M2M模式禁用
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);			//使能传输完成中断	//配置ADC1的DMA中断
	
	NVIC_InitStructure.NVIC_IRQChannel=DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	ADC_DMACmd(ADC1,ENABLE);		//使能ADC1的DMA	
	ADC_Cmd(ADC1, ENABLE);			//使能指定的ADC1	
	ADC_ResetCalibration(ADC1);	//重置指定的ADC1的校准寄存器 
	while(ADC_GetResetCalibrationStatus(ADC1));	//获取ADC1重置校准寄存器的状态,设置状态则等待	
	ADC_StartCalibration(ADC1);									//开始指定ADC1的校准状态
	while(ADC_GetCalibrationStatus(ADC1));			//获取指定ADC1的校准程序,设置状态则等待 

	DMA_Cmd(DMA1_Channel1,ENABLE);//使能DMA	
}

void  Photoresistor_Init(void)
{    
	ADC_InitTypeDef		ADC_InitStructure; 
	GPIO_InitTypeDef	GPIO_InitStructure;
	DMA_InitTypeDef		DMA_InitStructure;//注：ADC为12位模数转换器，只有ADCConvertedValue的低12位有效
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);			//使能DMA时钟	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);	//使能ADC1通道时钟
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);		//ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//ADC1复位借宿

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);		//72M/6=12,ADC最大时间不能超过14M
	                  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	//PB1通道9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;					//模拟输入引脚
  GPIO_Init(GPIOB, &GPIO_InitStructure);								//初始化  		
	
	ADC_DeInit(ADC1);  //将外设 ADC1 的全部寄存器重设为缺省值
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;//独立的转换模式ADC_DUALMOD［3:0］=0000;
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;//关闭扫描模式因为只有一个通道
	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;//使能连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv= ADC_ExternalTrigConv_None;//软件转换模式
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//对齐方式，ADC为12位中，右对齐方式ADC_ALIGN=0;
	ADC_InitStructure.ADC_NbrOfChannel=1;//开启通道数，1个ADC_SQR1［23:20］=0000;	//ADC_SQR1［23:20］设置通道数目的选择
	ADC_Init(ADC1,&ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_9,1,ADC_SampleTime_1Cycles5);

	//配置ADC1的DMA通道
	DMA_DeInit(DMA1_Channel1);//开启DMA1的第一通道
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//DMA对应的外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr =(uint32_t)&u16ADCConvertedValue;//内存存储基地址
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//DMA的转换模式为SRC模式，由外设搬移到内存
	DMA_InitStructure.DMA_BufferSize=16;//DMA缓存大小，1个
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//接收一次数据后，设备地址禁止后移
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//关闭接收一次数据后，目标内存地址后移 DMA_MemoryInc_Disable
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//定义外设数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//DMA搬移数据尺寸，HalfWord就是为16位
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;			//循环转换模式
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;	//DMA优先级高
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;				//M2M模式禁用
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);			//使能传输完成中断	//配置ADC1的DMA中断
	
	NVIC_InitStructure.NVIC_IRQChannel=DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	ADC_DMACmd(ADC1,ENABLE);		//使能ADC1的DMA	
	ADC_Cmd(ADC1, ENABLE);			//使能指定的ADC1	
	ADC_ResetCalibration(ADC1);	//重置指定的ADC1的校准寄存器 
	while(ADC_GetResetCalibrationStatus(ADC1));	//获取ADC1重置校准寄存器的状态,设置状态则等待	
	ADC_StartCalibration(ADC1);									//开始指定ADC1的校准状态
	while(ADC_GetCalibrationStatus(ADC1));			//获取指定ADC1的校准程序,设置状态则等待 

	DMA_Cmd(DMA1_Channel1,ENABLE);//使能DMA	
}

//中断处理函数
void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1)!=RESET)
	{
		ADC_SoftwareStartConvCmd(ADC1, DISABLE);	
		u16ADCOK = 0x55D5;
		DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
}

u16 Get_Average(u16 *pDatStartAdd,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val += *pDatStartAdd;
		++pDatStartAdd;
	}
	return temp_val/times;
}
