#include "adc.h"
#include "delay.h"

#define ADC1_DR_Address				0x4001244C

extern uint16_t u16ADCConvertedValue[],u16ADCOK;
//////////////////////////////////////////////////////////////////////////////////	 								  
////////////////////////////////////////////////////////////////////////////////// 	
//��ʼ��ADC															   
void  Adc_Init(void)
{    
	ADC_InitTypeDef		ADC_InitStructure; 
	GPIO_InitTypeDef	GPIO_InitStructure;
	DMA_InitTypeDef		DMA_InitStructure;//ע��ADCΪ12λģ��ת������ֻ��ADCConvertedValue�ĵ�12λ��Ч
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);			//ʹ��DMAʱ��	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);	//ʹ��ADC1ͨ��ʱ��
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);		//ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//ADC1��λ����

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);		//72M/6=12,ADC���ʱ�䲻�ܳ���14M
	
  //��ʼ��ADC1ͨ��3��7 IO�� //ADC123_IN3//ADC12_IN7                        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	//PA3ͨ��3��PA7ͨ��7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;					//ģ����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);								//��ʼ��  		
	
	ADC_DeInit(ADC1);  //������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;//������ת��ģʽADC_DUALMOD��3:0��=0000;
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;//�ر�ɨ��ģʽ��Ϊֻ��һ��ͨ��
	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;//ʹ������ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv= ADC_ExternalTrigConv_None;//���ת��ģʽ
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//���뷽ʽ��ADCΪ12λ�У��Ҷ��뷽ʽADC_ALIGN=0;
	ADC_InitStructure.ADC_NbrOfChannel=1;//����ͨ������1��ADC_SQR1��23:20��=0000;	//ADC_SQR1��23:20������ͨ����Ŀ��ѡ��
	ADC_Init(ADC1,&ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,1,ADC_SampleTime_1Cycles5);

	//����ADC1��DMAͨ��
	DMA_DeInit(DMA1_Channel1);//����DMA1�ĵ�һͨ��
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//DMA��Ӧ���������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr =(uint32_t)&u16ADCConvertedValue;//�ڴ�洢����ַ
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//DMA��ת��ģʽΪSRCģʽ����������Ƶ��ڴ�
	DMA_InitStructure.DMA_BufferSize=16;//DMA�����С��1��
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//����һ�����ݺ��豸��ַ��ֹ����
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//�رս���һ�����ݺ�Ŀ���ڴ��ַ���� DMA_MemoryInc_Disable
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//�����������ݿ��Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//DMA�������ݳߴ磬HalfWord����Ϊ16λ
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;			//ѭ��ת��ģʽ
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;	//DMA���ȼ���
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;				//M2Mģʽ����
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);			//ʹ�ܴ�������ж�	//����ADC1��DMA�ж�
	
	NVIC_InitStructure.NVIC_IRQChannel=DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	ADC_DMACmd(ADC1,ENABLE);		//ʹ��ADC1��DMA	
	ADC_Cmd(ADC1, ENABLE);			//ʹ��ָ����ADC1	
	ADC_ResetCalibration(ADC1);	//����ָ����ADC1��У׼�Ĵ��� 
	while(ADC_GetResetCalibrationStatus(ADC1));	//��ȡADC1����У׼�Ĵ�����״̬,����״̬��ȴ�	
	ADC_StartCalibration(ADC1);									//��ʼָ��ADC1��У׼״̬
	while(ADC_GetCalibrationStatus(ADC1));			//��ȡָ��ADC1��У׼����,����״̬��ȴ� 

	DMA_Cmd(DMA1_Channel1,ENABLE);//ʹ��DMA	
}

void  Photoresistor_Init(void)
{    
	ADC_InitTypeDef		ADC_InitStructure; 
	GPIO_InitTypeDef	GPIO_InitStructure;
	DMA_InitTypeDef		DMA_InitStructure;//ע��ADCΪ12λģ��ת������ֻ��ADCConvertedValue�ĵ�12λ��Ч
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);			//ʹ��DMAʱ��	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);	//ʹ��ADC1ͨ��ʱ��
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);		//ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//ADC1��λ����

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);		//72M/6=12,ADC���ʱ�䲻�ܳ���14M
	                  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	//PB1ͨ��9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;					//ģ����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);								//��ʼ��  		
	
	ADC_DeInit(ADC1);  //������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;//������ת��ģʽADC_DUALMOD��3:0��=0000;
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;//�ر�ɨ��ģʽ��Ϊֻ��һ��ͨ��
	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;//ʹ������ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv= ADC_ExternalTrigConv_None;//���ת��ģʽ
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//���뷽ʽ��ADCΪ12λ�У��Ҷ��뷽ʽADC_ALIGN=0;
	ADC_InitStructure.ADC_NbrOfChannel=1;//����ͨ������1��ADC_SQR1��23:20��=0000;	//ADC_SQR1��23:20������ͨ����Ŀ��ѡ��
	ADC_Init(ADC1,&ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_9,1,ADC_SampleTime_1Cycles5);

	//����ADC1��DMAͨ��
	DMA_DeInit(DMA1_Channel1);//����DMA1�ĵ�һͨ��
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//DMA��Ӧ���������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr =(uint32_t)&u16ADCConvertedValue;//�ڴ�洢����ַ
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//DMA��ת��ģʽΪSRCģʽ����������Ƶ��ڴ�
	DMA_InitStructure.DMA_BufferSize=16;//DMA�����С��1��
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//����һ�����ݺ��豸��ַ��ֹ����
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//�رս���һ�����ݺ�Ŀ���ڴ��ַ���� DMA_MemoryInc_Disable
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//�����������ݿ��Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//DMA�������ݳߴ磬HalfWord����Ϊ16λ
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;			//ѭ��ת��ģʽ
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;	//DMA���ȼ���
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;				//M2Mģʽ����
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);			//ʹ�ܴ�������ж�	//����ADC1��DMA�ж�
	
	NVIC_InitStructure.NVIC_IRQChannel=DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	ADC_DMACmd(ADC1,ENABLE);		//ʹ��ADC1��DMA	
	ADC_Cmd(ADC1, ENABLE);			//ʹ��ָ����ADC1	
	ADC_ResetCalibration(ADC1);	//����ָ����ADC1��У׼�Ĵ��� 
	while(ADC_GetResetCalibrationStatus(ADC1));	//��ȡADC1����У׼�Ĵ�����״̬,����״̬��ȴ�	
	ADC_StartCalibration(ADC1);									//��ʼָ��ADC1��У׼״̬
	while(ADC_GetCalibrationStatus(ADC1));			//��ȡָ��ADC1��У׼����,����״̬��ȴ� 

	DMA_Cmd(DMA1_Channel1,ENABLE);//ʹ��DMA	
}

//�жϴ�����
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
