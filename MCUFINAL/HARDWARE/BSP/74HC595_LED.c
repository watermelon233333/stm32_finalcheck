#include "74HC595_LED.h"
#include "delay.h"
//#include "stm32f4xx_gpio.h"
//#include "gpio.h"
extern uint8_t u8DispBuf[5];
extern uint8_t value[5];

//HC595显示接口
//#define HC595_SER				1<<0 	        //P1.0	数据信号
//#define HC595_SCK				1<<4	        //P1.4  数据时钟，上升沿有效
//#define HC595_RCK				1<<9          //P1.9  锁存输出时钟，上升沿有效
//#define HC595_NG				1<<14         //P1.14	输出使能，低电平有效
#define	LED_DIG1_LATCH_1()	PCout(4)=1	//PC4 DIG1显示
#define	LED_DIG1_LATCH_0()	PCout(4)=0	//PC4 DIG1关闭
#define	LED_DIG2_LATCH_1()	PBout(0)=1	//PB0 DIG2显示
#define	LED_DIG2_LATCH_0()	PBout(0)=0	//PB0 DIG2关闭
#define	LED_DIG3_LATCH_1()	PEout(7)=1	//PE7 DIG3显示
#define	LED_DIG3_LATCH_0()	PEout(7)=0	//PE7 DIG3关闭
#define	LED_DIG4_LATCH_1()	PAout(6)=1	//PA6 DIG4显示
#define	LED_DIG4_LATCH_0()	PAout(6)=0	//PA6 DIG4关闭

// stm32 <->  595
#define	HC595_A_1()			PEout(9)=1	//PE9
#define	HC595_A_0()			PEout(9)=0
#define	HC595_SCLK_1()	PEout(15)=1	//PE15 rising edge 0->7
#define	HC595_SCLK_0()	PEout(15)=0
#define	HC595_LATCH_1()	PEout(13)=1	//PE13 rising edge
#define	HC595_LATCH_0()	PEout(13)=0
#define	HC595_NG_1()		PEout(11)=1	//PE11 
#define	HC595_NG_0()		PEout(11)=0

void Init74HC595(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE, ENABLE);//使能GPIOE时钟
	
  //GPIOE7、GPIOE9、GPIOE11、GPIOE13、GPIOE15初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 |GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//推挽输出
  //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
  //GPIO_InitStructure.GPIO_PuPd = GPIO_Mode_IPU;//上拉
  GPIO_Init(GPIOE,&GPIO_InitStructure);//初始化

	GPIO_ResetBits(GPIOE,GPIO_Pin_7 |GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_15);//GPIOE9、GPIOE11、GPIOE13、GPIOE15清零	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOA,&GPIO_InitStructure);//初始化
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);//GPIOA6清零	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOB,&GPIO_InitStructure);//初始化
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);//GPIOB0清零	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOC,&GPIO_InitStructure);//初始化	
	GPIO_ResetBits(GPIOC,GPIO_Pin_4);//GPIOC4清零	
}

void HC595_Shift8bit_MSB_First(unsigned char datum)
{
	uint8_t i;
	HC595_LATCH_0();
	for(i=0;i<8;i++)
	{
		HC595_SCLK_0();
		delay_us(10);
		if(datum&0x80)	HC595_A_1();
		else	HC595_A_0();
		HC595_SCLK_1();
		delay_us(10);
		datum<<=1;
	}
//	HC595_LATCH_0();
	HC595_LATCH_1();
}

void HC595_Shift8bit_LSB_First(unsigned char datum)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		HC595_SCLK_0();
		delay_us(10);
		if(datum & 0x01)	HC595_A_1();
		else	HC595_A_0();
		HC595_SCLK_1();
		delay_us(10);
		datum>>=1;
	}
	HC595_LATCH_0();
	HC595_LATCH_1();
}

void HC595_addr_dat(char addr,char dat)
{
	LED_DIG1_LATCH_0();
	LED_DIG2_LATCH_0();
	LED_DIG3_LATCH_0();
	LED_DIG4_LATCH_0();
	
	HC595_NG_0();
//	HC595_LATCH_0();
//	delay_us(10);
	HC595_Shift8bit_MSB_First(dat);
//	delay_us(10);
//	HC595_Shift8bit_LSB_First(addr);
//	HC595_LATCH_1();
//	delay_us(10);
//	HC595_LATCH_0();
	
	if(addr==1)	LED_DIG1_LATCH_1();
	else if(addr==2)	LED_DIG2_LATCH_1();
	else if(addr==3)	LED_DIG3_LATCH_1();
	else if(addr==4)	LED_DIG4_LATCH_1();
}

//void Set74595(uint16_t data)
//{
//	uint8_t i;
//	for(i=0;i<16;i++)
//	{
//		HC595_SCLK_0();
//		//HC595Delay();
//		if(data&0x8000)
//			HC595_A_1();
//		else
//			HC595_A_0();
//		HC595_SCLK_1();
//		//HC595Delay();
//		data<<=1;
//	}
//	HC595_LATCH_0();
//	HC595_LATCH_1();
//}

/*F********************************************************************************************/
/* 函数名称: DispUpdate()                                                                     */
/* 功能描述:                                                                                  */
/*                                                                                            */
/* 输入参数: 无                                                                               */
/* 输出参数: 无                                                                               */
/*                                                                                            */
/* 全局变量:                                                                                  */
/* 调用模块:                                                                                  */
/* 其    他:                                                                                  */
/* 编 写 者: 霍 凯                                                                            */
/* 日    期: 2005-11-22                                                                       */
/**********************************************************************************************/
void DispUpdate(void)
{
		LED_DIG1_LATCH_0();
		LED_DIG2_LATCH_0();
		LED_DIG3_LATCH_0();
		LED_DIG4_LATCH_0();
		
		HC595_NG_0();
		HC595_Shift8bit_MSB_First(FontsTableCathode(u8DispBuf[1]));
		LED_DIG1_LATCH_1();
		delay_ms(1);
		LED_DIG1_LATCH_0();
	
		HC595_Shift8bit_MSB_First(FontsTableCathode(u8DispBuf[2]));
		LED_DIG2_LATCH_1();
		delay_ms(1);
		LED_DIG2_LATCH_0();

		HC595_Shift8bit_MSB_First(FontsTableCathode(u8DispBuf[3]));
		LED_DIG3_LATCH_1();
		delay_ms(1);
		LED_DIG3_LATCH_0();

		HC595_Shift8bit_MSB_First(FontsTableCathode(u8DispBuf[4]));
		LED_DIG4_LATCH_1();
		delay_ms(1);
		LED_DIG4_LATCH_0();

//    signed char i;
///*    
//	// 发送成功
//	//每次数据转储后，在24C256的0x00开始处存储修改后usSaveFirst、usSaveIndex
//	union_type.u16Buf[0] = usSaveFirst;
//	union_type.u16Buf[1] = usSaveIndex;
//	union_type.u16Buf[2] = usSaveCount;
//	I2C_WriteNByte(AT24C64,TWO_BYTE_SUBA,0x00,(unsigned char *)&union_type.u8Buf,6);
//	DelayXms(100);

//	timer0_counter = 0;
//*/
//	u8DispBuf[4]= u16CoinCount/1000;
//	u8DispBuf[3]= (u16CoinCount/100)%10;
//	u8DispBuf[2]= (u16CoinCount/10)%10;	
//	u8DispBuf[1]= u16CoinCount%10;
//	for(i=1;i<5;i++)
//	{
////	    HC595_Display(i,u8DispBuf[i]);
//	}
//	
}  // void DispUpdate(void)
void DispUpdate_new(void)
{
		LED_DIG1_LATCH_0();
		LED_DIG2_LATCH_0();
		LED_DIG3_LATCH_0();
		LED_DIG4_LATCH_0();
		
		HC595_NG_0();
		HC595_Shift8bit_MSB_First(value[1]);
		LED_DIG1_LATCH_1();
		delay_ms(1);
		LED_DIG1_LATCH_0();
	
		HC595_Shift8bit_MSB_First(value[2]);
		LED_DIG2_LATCH_1();
		delay_ms(1);
		LED_DIG2_LATCH_0();

		HC595_Shift8bit_MSB_First(value[3]);
		LED_DIG3_LATCH_1();
		delay_ms(1);
		LED_DIG3_LATCH_0();

		HC595_Shift8bit_MSB_First(value[4]);
		LED_DIG4_LATCH_1();
		delay_ms(1);
		LED_DIG4_LATCH_0();

}
/*********************************************************************  
*
* 函 数 名:   HC595_Shift8bit_MSB_First()
* 功能描述:   
* 函数说明: 
* 调用函数: 
* 全局变量:
* 输    入: 
* 返    回: 
* 设 计 者：                         	日期：
* 修 改 者：                        	日期：
* 版    本：
***********************************************************************/
//void HC595_Shift8bit_MSB_First(unsigned char datum)
//{
//    unsigned char t=8;

//	IOCLR1 |= HC595_SCK;

//	do
//	{
//	    if(datum & 0x80)
//	    	IOSET1 |= HC595_SER;
//	    else
//	    	IOCLR1 |= HC595_SER;
//		datum <<= 1;
//		DelayXus(10);
//		IOSET1 |= HC595_SCK;
//		DelayXus(10);
//		IOCLR1 |= HC595_SCK;
////		DelayXus(10);
//	}  while (--t!=0); 
//}
/*********************************************************************  
*
* 函 数 名:   HC595_Shift8Bit_LSB_First()
* 功能描述:   
* 函数说明: 
* 调用函数: 
* 全局变量:
* 输    入: 
* 返    回: 
* 设 计 者：                         	日期：
* 修 改 者：                        	日期：
* 版    本：
***********************************************************************/
//void HC595_Shift8bit_LSB_First(unsigned char datum)
//{
//    unsigned char t=8;

//	IOCLR1 |= HC595_SCK;

//	do
//	{
//	    if(datum & 0x01)
//	    	IOSET1 |= HC595_SER;
//	    else
//	    	IOCLR1 |= HC595_SER;
//		datum >>= 1;
//		DelayXus(10);
//		IOSET1 |= HC595_SCK;
//		DelayXus(10);
//		IOCLR1 |= HC595_SCK;
////		DelayXus(10);
//	}  while (--t!=0);
//}
/*********************************************************************  
*
* 函 数 名:   HC595_addr_dat()
* 功能描述:   
* 函数说明: 
* 调用函数: 
* 全局变量:
* 输    入: 
* 返    回: 
* 设 计 者：                         	日期：
* 修 改 者：                        	日期：
* 版    本：
***********************************************************************/
//void HC595_addr_dat(char addr,char dat)
//{
//	IOCLR1 |= HC595_NG;
//	IOCLR1 |= HC595_RCK;
//	DelayXus(10);
//	HC595_Shift8bit_MSB_First(dat);
//	DelayXus(10);
//	HC595_Shift8bit_LSB_First(addr);
//	IOSET1 |= HC595_RCK;
//	DelayXus(10);
//	IOCLR1 |= HC595_RCK;
//}

//-F--------------------------------------------------------------------------------------------
// 函数名称: FontsTableCathode()
// 功能描述: 共阴极字形码：dp g f e d c b a
//
// 输入参数: 无
// 输出参数: 无
//
// 全局变量:
// 调用模块:
// 其    他:
// 编 写 者: 霍 凯
// 日    期: 2012-06-22
//----------------------------------------------------------------------------------------------
unsigned char FontsTableCathode(unsigned char value) 
{			 	 	
    unsigned char val;

    switch(value)
    {  
        case 0x00:
        case 0x30:
            val=0x3f;    //0
            break;       

        case 0x01:
        case 0x31:
            val=0x06;    //1
            break;									

        case 0x02:
        case 0x32:
            val=0x5b;    //2
            break;									

        case 0x03:
        case 0x33:
            val=0x4f;    //3
            break;									

        case 0x04:
        case 0x34:
            val=0x66;    //4
            break;									

        case 0x05:
        case 0x35:
            val=0x6d;    //5
            break;									

        case 0x06:
        case 0x36:
            val=0x7d;    //6
            break;									

        case 0x07:
        case 0x37:
            val=0x07;    //7
            break;									

        case 0x08:
        case 0x38:
            val=0x7f;    //8
            break;									

        case 0x09:
        case 0x39:
            val=0x6f;    //9
            break;									

        case 0x2D:
            val=0x40;    //-
            break;									

        case 0x2E:
            val=0x80;    //.
            break;									

        case 0x3D:
            val=0x48;    //=
            break;									

        case 0x0A:
        case 0x41:
            val=0x77;    //A
            break;									

        case 0x0C:
        case 0x43:
            val=0x39;    //C
            break;									

        case 0x0E:
        case 0x45:
            val=0x79;    //E
            break;									

        case 0x0F:
        case 0x46:
            val=0x71;    //F
            break;									

        case 0x47:
            val=0x7d;    //G
            break;									

        case 0x48:
            val=0x76;    //H
            break;									

        case 0x4A:
            val=0x1e;    //J
            break;									

        case 0x4C:
            val=0x38;    //L
            break;									

        case 0x4E:
            val=0x37;    //N
            break;									

        case 0x4F:
            val=0x3f;    //O
            break;									

        case 0x50:
            val=0x73;    //P
            break;									

        case 0x53:
            val=0x6d;    //S
            break;									

        case 0x55:
            val=0x3e;    //U
            break;									

        case 0x5F:
            val=0x08;    //_
            break;									

        case 0x0B:
        case 0x62:
            val=0x7c;    //b
            break;									

        case 0x63:
            val=0x58;    //c
            break;									

        case 0x0D:
        case 0x64:
            val=0x5e;    //d
            break;									

        case 0x65:
            val=0x7b;    //e
            break;									

        case 0x67:
            val=0x6f;    //g
            break;									

        case 0x68:
            val=0x74;    //h
            break;									

        case 0x69:
            val=0x10;    //i
            break;									

        case 0x6E:
            val=0x54;    //n
            break;									

        case 0x6F:
            val=0x5c;    //o
            break;									

        case 0x70:
            val=0x73;    //p
            break;									

        case 0x71:
            val=0x67;    //q
            break;									

        case 0x72:
            val=0x50;    //r
            break;									

        case 0x74:
            val=0x78;    //t
            break;									

        case 0x75:
            val=0x1c;    //u
            break;									

        case 0x79:
            val=0x6e;    //y
            break;									

        case 0x7F:
        default:
            val=0x00;    //dark	
    }
    return (val);
}


/*********************************************************************  
*
* 函 数 名:   HC595_Display()
*
* 功能描述:   共阳极字形码   
* 函数说明: 
* 调用函数: 
* 全局变量:
* 输    入: 
* 返    回: 
* 设 计 者：                         	日期：
* 修 改 者：                        	日期：
* 版    本：
***********************************************************************/
//unsigned char HC595_Display_Table(unsigned char value) 
//{			 	 	
//    unsigned char val;
//    switch(value)
//    {  
//        case 0x00:
//        case 0x30:
//        {
//            val=0xc0;    //0		dp g f e d c b a
//            break;       
//        }
//        case 0x01:
//        case 0x31:
//        {
//            val=0xf9;    //1
//            break;									
//        }	
//        case 0x02:
//        case 0x32:
//        {
//            val=0xa4;    //2
//            break;									
//        }
//        case 0x03:
//        case 0x33:
//        {
//            val=0xb0;    //3
//            break;									
//        }	
//        case 0x04:
//        case 0x34:
//        {	
//            val=0x99;    //4
//            break;									
//        }
//        case 0x05:
//        case 0x35:
//        {
//            val=0x92;    //5
//            break;									
//        }
//        case 0x06:
//        case 0x36:
//        {
//            val=0x82;    //6
//            break;									
//        }	
//        case 0x07:
//        case 0x37:
//        {	
//            val=0xf8;    //7
//            break;									
//        }
//        case 0x08:
//        case 0x38:
//        {
//            val=0x80;    //8
//            break;									
//        }	
//        case 0x09:
//        case 0x39:
//        {			
//            val=0x90;    //9
//            break;									
//        }
//        case 0x2D:
//        {
//            val=0xbf;    //-
//            break;									
//        }
//        case 0x2E:
//        {
//            val=0x7f;    //.
//            break;									
//        }
//        case 0x3D:
//        {
//            val=0x09;    //=
//            break;									
//        }							
//        case 0x0A:
//        case 0x41:
//        {
//            val=0x88;    //A
//            break;									
//        }							
//        case 0x0C:
//        case 0x43:
//        {	
//            val=0xc6;    //C
//            break;									
//        }
//        case 0x0E:
//        case 0x45:
//        {		
//            val=0x86;    //E
//            break;									
//        }
//        case 0x0F:
//        case 0x46:
//        {
//            val=0x8e;    //F
//            break;									
//        }
//        case 0x47:
//        {
//            val=0x82;    //G
//            break;									
//        }			
//        case 0x48:
//        {
//            val=0x89;    //H
//            break;									
//        }
//        case 0x4A:
//        {
//            val=0x3D;    //J
//            break;									
//        }			
//        case 0x4C:
//        {
//            val=0x0E;    //L
//            break;									
//        }	
//        case 0x4E:
//        {
//            val=0x76;    //N
//            break;									
//        }	
//        case 0x4F:
//        {
//            val=0xc0;    //O
//            break;									
//        }	
//        case 0x50:
//        {
//            val=0x0c;    //P
//            break;									
//        }	
//        case 0x53:
//        {
//            val=0x92;    //S
//            break;									
//        }	
//        case 0x55:
//        {
//            val=0x3E;    //U
//            break;									
//        }
//        case 0x5F:
//        {
//            val=0x08;    //_
//            break;									
//        }				
//        case 0x0B:
//        case 0x62:
//        {
//            val=0x83;    //b
//            break;									
//        }	
//        case 0x63:
//        {
//            val=0x0D;    //c
//            break;									
//        }
//        case 0x0D:
//        case 0x64:
//        {
//            val=0x3D;    //d
//            break;									
//        }
//        case 0x65:
//        {
//            val=0x6F;    //e
//            break;									
//        }
//        case 0x67:
//        {
//            val=0x90;    //g
//            break;									
//        }	
//        case 0x68:
//        {
//            val=0x17;    //h
//            break;									
//        }
//        case 0x69:
//        {
//            val=0x10;    //i
//            break;									
//        }	
//        case 0x6E:
//        {
//            val=0x15;    //n
//            break;									
//        }
//        case 0x6F:
//        {
//            val=0x1D;    //o
//            break;									
//        }	
//        case 0x70:
//        {
//            val=0x0c;    //p
//            break;									
//        }
//        case 0x71:
//        {
//            val=0x73;    //q
//            break;									
//        }	
//        case 0x72:
//        {
//            val=0x05;    //r
//            break;									
//        }
//        case 0x74:
//        {
//            val=0x0F;    //t
//            break;									
//        }	
//        case 0x75:
//        {
//            val=0x1C;    //u
//            break;									
//        }
//        case 0x79:
//        {
//            val=0x3B;    //y
//            break;									
//        }
//        case LED_SEG_A:
//        {
//            val=(1<<0x06);   //LED_A
//            break;									
//        }
//        case LED_SEG_B:
//        {
//            val=(1<<0x05);   //LED_B
//            break;									
//        }
//        case LED_SEG_C:
//        {
//            val=(1<<0x04);   //LED_C
//            break;									
//        }
//        case LED_SEG_D:
//        {
//            val=(1<<0x03);   //LED_D
//            break;									
//        }
//        case LED_SEG_E:
//        {
//            val=(1<<0x02);   //LED_E
//            break;									
//        }
//        case LED_SEG_F:
//        {
//            val=(1<<0x01);   //LED_F
//            break;									
//        }
//        case LED_SEG_G:
//        {
//            val=0x01;        //LED_G
//            break;									
//        }
//        case LED_SEG_DP:
//        {
//            val=(1<<0x07);   //LED_DP
//            break;									
//        }
//        case 0x7F:
//        default:
//        { 
//            val=0xff;        //dark	
//        }			
//    }
//	return(val);		
////    HC595_addr_dat(addr,val);
//}
/*********************************************************************  
*
* 函 数 名:   Display_Init()
*
* 功能描述:   共阳极字形码   
* 函数说明: 
* 调用函数: 
* 全局变量:
* 输    入: 
* 返    回: 
* 设 计 者：                         	日期：
* 修 改 者：                        	日期：
* 版    本：
***********************************************************************/
//void DispInit(void)
//{
///*
//    HC595_Display(0x01,'_');
//	HC595_Display(0x02,'_');
//	HC595_Display(0x03,'_');
//	HC595_Display(0x04,'_');
//*/

//}
/*********************************************************************  
*
* 函 数 名:   DelayXus()
* 功能描述:   微秒延时
* 函数说明: 
* 调用函数: 
* 全局变量:
* 输    入: 
* 返    回: 
* 设 计 者：                         	日期：
* 修 改 者：                        	日期：
* 版    本：
***********************************************************************/
//void DelayXus(unsigned long u32Xus) //延时Xus
//{
//    unsigned long i=u32Xus;
//   	while (--i!= 0);
//}
/*********************************************************************  
*
* 函 数 名:   DelayXms()
* 功能描述:   毫秒延时
* 函数说明: 
* 调用函数: 
* 全局变量:
* 输    入: 
* 返    回: 
* 设 计 者：                         	日期：
* 修 改 者：                        	日期：
* 版    本：
***********************************************************************/
//void DelayXms (unsigned long u32Xms)
//{
//    unsigned long i=u32Xms,j;
//   	while (--i != 0)
//		for (j=0; j<500; j++);
//}
