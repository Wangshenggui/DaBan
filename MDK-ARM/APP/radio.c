#include "radio.h"


void RadioInit()
{
    SetRadioMode(RadioGeneralModel);//һ��ģʽ
//    SetRadioMode(RadioWakeupMode);//����ģʽ
//    SetRadioMode(RadioEcoMode);//ʡ��ģʽ
//    SetRadioMode(RadioSleepMode);//����ģʽ
}

/*
c0 00 00 1a 17 40

c0 00 01 1a 17 40
*/

//���ù���ģʽ
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
//��������
void RadioSendCMD()
{
    
}








