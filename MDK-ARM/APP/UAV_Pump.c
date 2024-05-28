#include "UAV_Pump.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "system.h"

//50Hz->20ms->20000us

extern osSemaphoreId PumpOffBinSemHandle;

uint8_t InputCapIndex = 0;//��¼������ʽ
uint16_t InputCap_n = 0;
uint16_t InputCap_n1 = 0;
uint16_t InputCap_n2 = 0;
uint8_t InputCapFlag = 0;//�ɼ���ɱ�־

static uint8_t stop_i = 0;

//�����ж�
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim)
{
    if (htim == UAV_PUMP_TIM)
    {
        //������
        if (InputCapIndex == 0)
        {
            InputCapIndex = 1;//�л��½���
            InputCap_n1 = UAV_PUMP_TIM_CCR;//��ȡ����ֵ
            //�л�Ϊ�½��ز���
            __HAL_TIM_SET_CAPTUREPOLARITY(UAV_PUMP_TIM, UAV_PUMP_TIM_CH, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else if (InputCapIndex == 1)
        {
            InputCapIndex = 0;//�л�������
            InputCap_n2 = UAV_PUMP_TIM_CCR;//��ȡ����ֵ
			
			UAV_PUMP_TIM_CCR=0;
			TIM3->CNT=0;

            //���
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
            //1000->�ر�     2000->����
            if (InputCap_n > (2000 - Err) && InputCap_n < (2000 + Err))
            {
                RunGuidance_Struct.SysBeng = 1;
            }
            else
            {
                RunGuidance_Struct.SysBeng = 0;
                osSemaphoreRelease(PumpOffBinSemHandle);//�ͷ�ֹͣ�ź���
            }
            
            //InputCap_n = 0;

            //�л�Ϊ�����ز���
            __HAL_TIM_SET_CAPTUREPOLARITY(UAV_PUMP_TIM, UAV_PUMP_TIM_CH, TIM_INPUTCHANNELPOLARITY_RISING);
        }
        stop_i = 0;
    }
}


//10ms
void PumpDisconnectDetection()
{
//    //û���յ��ź�
//    if (InputCap_n == 0)
//    {
//        stop_i++;
//        //1sû���ź�
//        if (stop_i == 100)
//        {
//            stop_i = 0;
//            //��Ϊû�н���
//            RunGuidance_Struct.SysBeng = 0;
//            InputCap_n = 0;

//            osSemaphoreRelease(PumpOffBinSemHandle);//�ͷ�ֹͣ�ź���
//        }
//    }
//    else
//    {
//        stop_i = 0;
//    }
}









