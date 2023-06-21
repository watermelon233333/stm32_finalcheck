/********************金逸晨**************************请务必外接SCK,SDA上拉电阻
*****************0.96  4PIN IIC OLED FOR STM32F103*************
*****STM32F103驱动**************************
***** PB5----3  SCK   ,  PB6----4  SDA   ,    
***** BY:GU 
********************************************************/
#include "delay.h"
#include "myiic.h"   
#include "OLED4PINiic.h"
#include "oledfont.h" 

unsigned char ACK=0;
u8 OLED_GRAM[128][8];	 

const unsigned char  OLED_init_cmd[25]=
{
  /*0xae,0X00,0X10,0x40,0X81,0XCF,0xff,0xa1,0xa4,
  0xA6,0xc8,0xa8,0x3F,0xd5,0x80,0xd3,0x00,0XDA,0X12,
  0x8d,0x14,0xdb,0x40,0X20,0X02,0xd9,0xf1,0xAF*/
	0xAE,	//关闭显示
	0xD5,	//设置时钟分频因子,震荡频率
	0x80,	//[3:0],分频因子;[7:4],震荡频率

	0xA8,	//设置驱动路数
	0X3F,	//默认0X3F(1/64)
	0xD3,	//设置显示偏移
	0X00,	//默认为0
	0x40,	//设置显示开始行 [5:0],行数.                              
	0x8D,	//电荷泵设置
	0x14,	//bit2，开启/关闭
	0x20,	//设置内存地址模式
	0x02,	//[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	0xA1,	//段重定义设置,bit0:0,0->0;1,0->127;
	0xC0,	//设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	0xDA,	//设置COM硬件引脚配置
	0x12,	//[5:4]配置            
	0x81,	//对比度设置
	0xEF,	//1~255;默认0X7F (亮度设置,越大越亮)
	0xD9,	//设置预充电周期
	0xf1,	//[3:0],PHASE 1;[7:4],PHASE 2;
	0xDB,	//设置VCOMH 电压倍率
	0x30,	//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	0xA4,	//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	0xA6,	//设置显示方式;bit0:1,反相显示;0,正常显示        
	0xAF,	//开启显示     
};


void OLED_send_cmd(unsigned char o_command)
{
	IIC_Start();  	 	   
	IIC_Send_Byte(0x78);     //发送写命令			   
	IIC_Wait_Ack();
	IIC_Send_Byte(0x00);     //发送写命令			   
	IIC_Wait_Ack();
	IIC_Send_Byte(o_command);//发送写命令			   
	IIC_Wait_Ack();
	IIC_Stop();						   //产生一个停止条件	 
	
//	IIC_start();
//	IIC_write(0x00); 
//	IIC_write(o_command);
//	IIC_stop();	
}
void OLED_send_data(unsigned char o_data)
{
	IIC_Start();  	 	   
	IIC_Send_Byte(0x78);  //发送写命令			   
	IIC_Wait_Ack();
	IIC_Send_Byte(0x40);  //发送写命令			   
	IIC_Wait_Ack();
	IIC_Send_Byte(o_data);//发送写命令			   
	IIC_Wait_Ack();
	IIC_Stop();						//产生一个停止条件	
	
//	IIC_start();
//	IIC_write(0x40);
//	IIC_write(o_data);
//	IIC_stop();
}
	
void OLED_Column_set(unsigned char column)
{
	OLED_send_cmd(0x10|(column>>4));	//设置列地址高位
	OLED_send_cmd(0x00|(column&0x0f));//设置列地址低位  
}

void OLED_Page_set(unsigned char page)
{
	OLED_send_cmd(0xb0+page);
}

void OLED_clear(void)
{
	unsigned char page,column;
	for(page=0;page<8;page++)	//page loop
	{ 
		OLED_Page_set(page);
		OLED_Column_set(0);	  
		for(column=0;column<128;column++)	//column loop
		{
			OLED_send_data(0x00);
		}
	}
}
	
void OLED_full(void)
{
	unsigned char page,column;
	for(page=0;page<8;page++)	//page loop
	{ 
		OLED_Page_set(page);
		OLED_Column_set(0);	  
		for(column=0;column<128;column++)	//column loop
		{
			OLED_send_data(0xff);
		}
	}
}

void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_send_cmd(0xB0 | Y);					//??Y??
	OLED_send_cmd(0x10 | ((X & 0xF0) >> 4));	//??X???4?
	OLED_send_cmd(0x00 | (X & 0x0F));			//??X???4?
}

void OLED_init(void)
{
	unsigned char i;
	for(i=0;i<25;i++)
		{
			OLED_send_cmd(OLED_init_cmd[i]);
		}
}

//更新显存到LCD		 
void OLED_Refresh_Gram(void)
{
	unsigned char page,column;
	for(page=0;page<8;page++)	//page loop
	{ 
		OLED_Page_set(page);
		OLED_Column_set(0);	  
		for(column=0;column<128;column++)	//column loop
		{
			OLED_send_data(OLED_GRAM[column][page]);
		}
	}	   
}

//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//更新显示
}

//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}

////////////////////////////////////////
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2)
{
	u8 t; 
	u8 xerr=0,yerr=0,delta_x,delta_y,distance; 
	u8 incx,incy,uRow,uCol; 
	uRow=x1; 
	uCol=y1;
	
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 

	
	if(delta_x>0)incx=1; 						//设置单步方向 
	else if(delta_x==0)incx=0;			//垂直线 
	else {incx=-1;delta_x=-delta_x;}
	
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;			//水平线 
	else{incy=-1;delta_y=-delta_y;}
	
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y;
	
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		OLED_DrawPoint(uRow,uCol,1);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}  

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void OLED_Draw_Circle(u8 x0,u8 y0,u8 r)
{
	s8 a,b;
	s8 di;
	a=0;b=r;	  
	di=3-(r<<1);	//判断下个点位置的标志
	while(a<=b)
	{
		OLED_DrawPoint(x0+a,y0-b,1);             //5
 		OLED_DrawPoint(x0+b,y0-a,1);             //0           
		OLED_DrawPoint(x0+b,y0+a,1);             //4               
		OLED_DrawPoint(x0+a,y0+b,1);             //6 
		OLED_DrawPoint(x0-a,y0+b,1);             //1       
 		OLED_DrawPoint(x0-b,y0+a,1);             
		OLED_DrawPoint(x0-a,y0-b,1);             //2             
  	OLED_DrawPoint(x0-b,y0-a,1);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)	di += 4*a+6;	  
		else
		{
			di += 10+4*(a-b);   
			b--;
		} 						    
	}
}

//画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void OLED_DrawRectangle(u8 x1, u8 y1, u8 x2, u8 y2)
{
	OLED_DrawLine(x1,y1,x2,y1);
	OLED_DrawLine(x1,y1,x1,y2);
	OLED_DrawLine(x1,y2,x2,y2);
	OLED_DrawLine(x2,y1,x2,y2);
}



//////////////////////////////////////////
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 12/16/24
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);	//得到字体一个字符对应点阵集所占的字节数
	chr=chr-' ';//得到偏移后的值	
    
	 for(t=0;t<csize;t++)
    {   
			if(size==12)temp=asc2_1206[chr][t]; 	 		//调用1206字体
			else if(size==16)temp=asc2_1608[chr][t];	//调用1608字体
			else if(size==24)temp=asc2_2412[chr][t];	//调用2412字体
			else return;															//没有的字库
			for(t1=0;t1<8;t1++)
			{
				if(temp&0x80)OLED_DrawPoint(x,y,mode);
				else OLED_DrawPoint(x,y,!mode);
				temp<<=1;
				y++;
				if((y-y0)==size)
				{
					y=y0;
					x++;
					break;
				}
			}  	 
    }          
}

void OLED_ShowChineseChar(u8 x,u8 y,u16 chr,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=32;	//得到字体一个字符对应点阵集所占的字节数
	if(chr==(u16)"朱") chr=0;
	else if(chr==(u16)"浩") chr=0;
	else if(chr==(u16)"然") chr=0;
    for(t=0;t<csize;t++)
    {   
			temp=CHS_1616[chr][t]; 	 		//调用1206字体
			
			for(t1=0;t1<8;t1++)
			{
				if(temp&0x80)OLED_DrawPoint(x,y,mode);
				else OLED_DrawPoint(x,y,!mode);
				temp<<=1;
				y++;
				if((y-y0)==16)
				{
					y=y0;
					x++;
					break;
				}
			}  	 
    }          
}
//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 
//显示字符串
//x,y:起点坐标  
//size:字体大小 
//*p:字符串起始地址 
void OLED_ShowString(u8 x,u8 y,const u8	*p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
	
}


////////////////////////////////////////////////////

void Picture_display(const unsigned char *ptr_pic)
{
	unsigned char page,column;
	for(page=0;page<(64/8);page++)	//page loop
	{ 
		OLED_Page_set(page);
		OLED_Column_set(0);	  
		for(column=0;column<128;column++)	//column loop
		{
			OLED_send_data(*ptr_pic++);
		}
	}
}
	
void Picture_ReverseDisplay(const unsigned char *ptr_pic)
{
	unsigned char page,column,data;
	for(page=0;page<(64/8);page++)	//page loop
	{ 
		OLED_Page_set(page);
		OLED_Column_set(0);	  
		for(column=0;column<128;column++)	//column loop
		{
			data=*ptr_pic++;
			data=~data;
			OLED_send_data(data);
		}
	}
}


void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;

	for(y=y1;y>=y0;y--)
	{
		OLED_SetCursor(y,x0);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_send_data(BMP[j++]);
	    }
	}
} 


