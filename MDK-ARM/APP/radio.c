#include "radio.h"


void RadioInit()
{
    SetRadioMode(RadioGeneralModel);//一般模式
//    SetRadioMode(RadioWakeupMode);//唤醒模式
//    SetRadioMode(RadioEcoMode);//省电模式
//    SetRadioMode(RadioSleepMode);//休眠模式
}

/*
c0 00 00 1a 17 40

c0 00 01 1a 17 40
*/

//设置工作模式
void SetRadioMode(uint8_t mode)
{
    switch(mode)
    {
        case(0):
            HAL_GPIO_WritePin(RadioM0_GPIO_Port,RadioM0_Pin,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(RadioM1_GPIO_Port,RadioM1_Pin,GPIO_PIN_RESET);
            break;
        case(1):
            HAL_GPIO_WritePin(RadioM0_GPIO_Port,RadioM0_Pin,GPIO_PIN_SET);
            HAL_GPIO_WritePin(RadioM1_GPIO_Port,RadioM1_Pin,GPIO_PIN_RESET);
            break;
        case(2):
            HAL_GPIO_WritePin(RadioM0_GPIO_Port,RadioM0_Pin,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(RadioM1_GPIO_Port,RadioM1_Pin,GPIO_PIN_SET);
            break;
        case(3):
            HAL_GPIO_WritePin(RadioM0_GPIO_Port,RadioM0_Pin,GPIO_PIN_SET);
            HAL_GPIO_WritePin(RadioM1_GPIO_Port,RadioM1_Pin,GPIO_PIN_SET);
            break;
    }
}
//发送命令
void RadioSendCMD()
{
    
}








