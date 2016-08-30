#ifndef __KEY_H
#define __KEY_H

#include <rtthread.h>
#include "stm32f10x.h"


extern u8 test_flag;
extern u8 time_10ms_ok;

void key_init(void);
u8 key_scan(void);
u8 KeyRead(void);
u8 Read_key(void);

void beep_init(void);
void beep_onoff(u8 onoff);


#endif
