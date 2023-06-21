#ifndef   __74HC595__H
#define   __74HC595__H
#include "sys.h"


void Init74HC595(void);
void HC595_addr_dat(char addr,char dat);
void DispUpdate(void);
unsigned char FontsTableCathode(unsigned char value);
void DispUpdate1(u8 x1,u8 x2,u8 x3,u8 x4);

#endif 
