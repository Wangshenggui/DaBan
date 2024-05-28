#ifndef _RADIO_H
#define _RADIO_H

#include "main.h"


//定义工作模式\
0：一般模式\
1：唤醒模式\
2：省电模式\
3：休眠模式
#define RadioGeneralModel 0
#define RadioWakeupMode 1
#define RadioEcoMode 2
#define RadioSleepMode 3

void RadioInit(void);
void SetRadioMode(uint8_t mode);









#endif
