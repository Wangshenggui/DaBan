#include "systick.h"
#include "FreeRTOS.h"
#include "task.h"


//��ʱnus
void delay_us(uint32_t nus)
{
    HAL_Delay(nus);
}

//��ʱnms
void delay_ms(uint32_t nus)
{
    //Ĭ�ϵ�ms
    HAL_SYSTICK_Config(SystemCoreClock / (1000U / uwTickFreq));
    HAL_Delay(nus);
    HAL_SYSTICK_Config(SystemCoreClock / 1000000);//����Ϊus
}












