/********************���ݳ�**************************��������SCK,SDA��������
*****************0.96  4PIN IIC OLED FOR STM32F103*************
*****STM32F103����**************************
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
	0xAE,	//�ر���ʾ
	0xD5,	//����ʱ�ӷ�Ƶ����,��Ƶ��
	0x80,	//[3:0],��Ƶ����;[7:4],��Ƶ��

	0xA8,	//��������·��
	0X3F,	//Ĭ��0X3F(1/64)
	0xD3,	//������ʾƫ��
	0X00,	//Ĭ��Ϊ0
	0x40,	//������ʾ��ʼ�� [5:0],����.                              
	0x8D,	//��ɱ�����
	0x14,	//bit2������/�ر�
	0x20,	//�����ڴ��ַģʽ
	0x02,	//[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	0xA1,	//���ض�������,bit0:0,0->0;1,0->127;
	0xC0,	//����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	0xDA,	//����COMӲ����������
	0x12,	//[5:4]����            
	0x81,	//�Աȶ�����
	0xEF,	//1~255;Ĭ��0X7F (��������,Խ��Խ��)
	0xD9,	//����Ԥ�������
	0xf1,	//[3:0],PHASE 1;[7:4],PHASE 2;
	0xDB,	//����VCOMH ��ѹ����
	0x30,	//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	0xA4,	//ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	0xA6,	//������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ        
	0xAF,	//������ʾ     
};


void OLED_send_cmd(unsigned char o_command)
{
	IIC_Start();  	 	   
	IIC_Send_Byte(0x78);     //����д����			   
	IIC_Wait_Ack();
	IIC_Send_Byte(0x00);     //����д����			   
	IIC_Wait_Ack();
	IIC_Send_Byte(o_command);//����д����			   
	IIC_Wait_Ack();
	IIC_Stop();						   //����һ��ֹͣ����	 
	
//	IIC_start();
//	IIC_write(0x00); 
//	IIC_write(o_command);
//	IIC_stop();	
}
void OLED_send_data(unsigned char o_data)
{
	IIC_Start();  	 	   
	IIC_Send_Byte(0x78);  //����д����			   
	IIC_Wait_Ack();
	IIC_Send_Byte(0x40);  //����д����			   
	IIC_Wait_Ack();
	IIC_Send_Byte(o_data);//����д����			   
	IIC_Wait_Ack();
	IIC_Stop();						//����һ��ֹͣ����	
	
//	IIC_start();
//	IIC_write(0x40);
//	IIC_write(o_data);
//	IIC_stop();
}
	
void OLED_Column_set(unsigned char column)
{
	OLED_send_cmd(0x10|(column>>4));	//�����е�ַ��λ
	OLED_send_cmd(0x00|(column&0x0f));//�����е�ַ��λ  
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

//�����Դ浽LCD		 
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

//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//������ʾ
}

//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}

////////////////////////////////////////
//����
//x1,y1:�������
//x2,y2:�յ�����  
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2)
{
	u8 t; 
	u8 xerr=0,yerr=0,delta_x,delta_y,distance; 
	u8 incx,incy,uRow,uCol; 
	uRow=x1; 
	uCol=y1;
	
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 

	
	if(delta_x>0)incx=1; 						//���õ������� 
	else if(delta_x==0)incx=0;			//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;}
	
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;			//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;}
	
	if(delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y;
	
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		OLED_DrawPoint(uRow,uCol,1);//���� 
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

//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void OLED_Draw_Circle(u8 x0,u8 y0,u8 r)
{
	s8 a,b;
	s8 di;
	a=0;b=r;	  
	di=3-(r<<1);	//�ж��¸���λ�õı�־
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
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)	di += 4*a+6;	  
		else
		{
			di += 10+4*(a-b);   
			b--;
		} 						    
	}
}

//������	  
//(x1,y1),(x2,y2):���εĶԽ�����
void OLED_DrawRectangle(u8 x1, u8 y1, u8 x2, u8 y2)
{
	OLED_DrawLine(x1,y1,x2,y1);
	OLED_DrawLine(x1,y1,x1,y2);
	OLED_DrawLine(x1,y2,x2,y2);
	OLED_DrawLine(x2,y1,x2,y2);
}



//////////////////////////////////////////
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//������ʾ
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 12/16/24
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);	//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ	
    
	 for(t=0;t<csize;t++)
    {   
			if(size==12)temp=asc2_1206[chr][t]; 	 		//����1206����
			else if(size==16)temp=asc2_1608[chr][t];	//����1608����
			else if(size==24)temp=asc2_2412[chr][t];	//����2412����
			else return;															//û�е��ֿ�
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
	u8 csize=32;	//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	if(chr==(u16)"��") chr=0;
	else if(chr==(u16)"��") chr=0;
	else if(chr==(u16)"Ȼ") chr=0;
    for(t=0;t<csize;t++)
    {   
			temp=CHS_1616[chr][t]; 	 		//����1206����
			
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
//m^n����
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
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
//��ʾ�ַ���
//x,y:�������  
//size:�����С 
//*p:�ַ�����ʼ��ַ 
void OLED_ShowString(u8 x,u8 y,const u8	*p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
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

