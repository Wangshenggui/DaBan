#ifndef _RADIO_H
#define _RADIO_H

#include "main.h"


//���幤��ģʽ\
0��һ��ģʽ\
1������ģʽ\
2��ʡ��ģʽ\
3������ģʽ
#define RadioGeneralModel 0
#define RadioWakeupMode 1
#define RadioEcoMode 2
#define RadioSleepMode 3

void RadioInit(void);
void SetRadioMode(uint8_t mode);









#endif
