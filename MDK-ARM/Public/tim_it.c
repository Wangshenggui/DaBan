#include "tim_it.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "UpperCom_USART.h"
#include "UAV_Pump.h"

#include "RTK_usart_it.h"


//��ʱ��7�ص�����
//10ms
void TIM7_PeriodElapsedCallback()
{
    static uint8_t i = 0;
    i++;
    if (i == 100)//1s
    {
        i = 0;
        //��¼����ʱ��
        RunTime += 1;
    }

	static uint8_t j = 0;
	j++;
    if (j == 20)//200ms
    {
        j = 0;
        SendG070_RTK_Data();
    }
	
    //10ms���
    PumpDisconnectDetection();
}

extern osSemaphoreId UpperComBinSemHandle;
extern//�ϴ���־λ
uint8_t UpFlag;
//��ʱ��4�ص�����
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
            osSemaphoreRelease(UpperComBinSemHandle);//�ͷ��ϴ��ź���
        }
    }
    
}












