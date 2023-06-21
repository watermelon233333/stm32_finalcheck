#ifndef   __74HC595__H
#define   __74HC595__H
#include "sys.h"


void Init74HC595(void);
void HC595_addr_dat(char addr,char dat);
void DispUpdate(void);
unsigned char FontsTableCathode(unsigned char value);


#endif 
