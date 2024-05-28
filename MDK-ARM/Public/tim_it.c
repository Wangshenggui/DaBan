#include "tim_it.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "UpperCom_USART.h"
#include "UAV_Pump.h"

#include "RTK_usart_it.h"


//定时器7回调处理
//10ms
void TIM7_PeriodElapsedCallback()
{
    static uint8_t i = 0;
    i++;
    if (i == 100)//1s
    {
        i = 0;
        //记录运行时间
        RunTime += 1;
    }

	static uint8_t j = 0;
	j++;
    if (j == 20)//200ms
    {
        j = 0;
        SendG070_RTK_Data();
    }
	
    //10ms检测
    PumpDisconnectDetection();
}

extern osSemaphoreId UpperComBinSemHandle;
extern//上传标志位
uint8_t UpFlag;
//定时器4回调处理
//10ms
void TIM4_PeriodElapsedCallback()
{
    static uint8_t t=0;
    
    t++;
    if(t==80)
    {
        t=0;
//        if(UpFlag==1)
        {
            osSemaphoreRelease(UpperComBinSemHandle);//释放上传信号量
        }
    }
    
}












