#include "systick.h"
#include "FreeRTOS.h"
#include "task.h"


//延时nus
void delay_us(uint32_t nus)
{
    HAL_Delay(nus);
}

//延时nms
void delay_ms(uint32_t nus)
{
    //默认的ms
    HAL_SYSTICK_Config(SystemCoreClock / (1000U / uwTickFreq));
    HAL_Delay(nus);
    HAL_SYSTICK_Config(SystemCoreClock / 1000000);//设置为us
}












