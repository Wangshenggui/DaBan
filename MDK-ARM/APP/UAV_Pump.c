#include "UAV_Pump.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "system.h"

//50Hz->20ms->20000us

extern osSemaphoreId PumpOffBinSemHandle;

uint8_t InputCapIndex = 0;//记录触发方式
uint16_t InputCap_n = 0;
uint16_t InputCap_n1 = 0;
uint16_t InputCap_n2 = 0;
uint8_t InputCapFlag = 0;//采集完成标志

static uint8_t stop_i = 0;

//捕获中断
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim)
{
    if (htim == UAV_PUMP_TIM)
    {
        //上升沿
        if (InputCapIndex == 0)
        {
            InputCapIndex = 1;//切换下降沿
            InputCap_n1 = UAV_PUMP_TIM_CCR;//获取计数值
            //切换为下降沿捕获
            __HAL_TIM_SET_CAPTUREPOLARITY(UAV_PUMP_TIM, UAV_PUMP_TIM_CH, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else if (InputCapIndex == 1)
        {
            InputCapIndex = 0;//切换上升沿
            InputCap_n2 = UAV_PUMP_TIM_CCR;//获取计数值
			
			UAV_PUMP_TIM_CCR=0;
			TIM3->CNT=0;

            //溢出
            if (InputCap_n2 < InputCap_n1)
            {
//                InputCap_n = InputCap_n2 + 0xffff - InputCap_n1;
				InputCap_n = 2000;
				return;
            }
            else
            {
                InputCap_n = InputCap_n2 - InputCap_n1;
            }

#define Err 300
            //1000->关闭     2000->开启
            if (InputCap_n > (2000 - Err) && InputCap_n < (2000 + Err))
            {
                RunGuidance_Struct.SysBeng = 1;
            }
            else
            {
                RunGuidance_Struct.SysBeng = 0;
                osSemaphoreRelease(PumpOffBinSemHandle);//释放停止信号量
            }
            
            //InputCap_n = 0;

            //切换为上升沿捕获
            __HAL_TIM_SET_CAPTUREPOLARITY(UAV_PUMP_TIM, UAV_PUMP_TIM_CH, TIM_INPUTCHANNELPOLARITY_RISING);
        }
        stop_i = 0;
    }
}


//10ms
void PumpDisconnectDetection()
{
//    //没有收到信号
//    if (InputCap_n == 0)
//    {
//        stop_i++;
//        //1s没有信号
//        if (stop_i == 100)
//        {
//            stop_i = 0;
//            //认为没有接入
//            RunGuidance_Struct.SysBeng = 0;
//            InputCap_n = 0;

//            osSemaphoreRelease(PumpOffBinSemHandle);//释放停止信号量
//        }
//    }
//    else
//    {
//        stop_i = 0;
//    }
}









