#include "Slave_USART.h"
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"


USART3_RxStructure USART3_RxStruct;

//�ӻ����
uint8_t Slave_number = 0x00;
uint8_t Slave_State[SlaveNum] = { 0 };
//��ȡ�����ٶ�
uint8_t ReadSpeed1[SlaveNum] = { 0 }, ReadSpeed2[SlaveNum] = { 0 };


//�����ж�
void USART3_IDLE_Handler()
{
    if (RESET != __HAL_UART_GET_FLAG(SlaveUART, UART_FLAG_IDLE))   //�ж��Ƿ��ǿ����ж�
    {
		
		
        __HAL_UART_CLEAR_IDLEFLAG(SlaveUART);                     //��������жϱ�־�������һֱ���Ͻ����жϣ�
        HAL_UART_DMAStop(SlaveUART);

        ///////////////////////
        //��ȡ����
        USART3_RxStruct.Rx_len = Rx_LENG - __HAL_DMA_GET_COUNTER(SlaveUART_DMA_RX);
        //�����ַ���
		
//        memset(Slave_State,0,SlaveNum);
//        memset(ReadSpeed1,0,SlaveNum);
//        memset(ReadSpeed2,0,SlaveNum);
        memset(USART3_RxStruct.Buff, 0, USART3_RxStruct.Rx_len);
        memcpy(USART3_RxStruct.Buff, USART3_RxStruct.Rx_Buff, USART3_RxStruct.Rx_len);
        //��ջ�����
        memset(USART3_RxStruct.Rx_Buff, 0, USART3_RxStruct.Rx_len);
        /////////////////////////////////////
        if (USART3_RxStruct.Buff[0] == 0xEB && USART3_RxStruct.Buff[5] == 0x90)
        {
            /*----------------------------------------------*/
            //�ӻ����͵�״̬\
                            0     1      2    3\
            ����          ��EB �ӻ���ַ ״̬  90
			Slave_State[Slave_number - 1] = 0;
			ReadSpeed1[Slave_number - 1] = 0;
			ReadSpeed2[Slave_number - 1] = 0;
            if (USART3_RxStruct.Buff[1] == Slave_number)
            {
                Slave_State[Slave_number - 1] = USART3_RxStruct.Buff[2];
                ReadSpeed1[Slave_number - 1] = USART3_RxStruct.Buff[3];
                ReadSpeed2[Slave_number - 1] = USART3_RxStruct.Buff[4];
				
				//��Ƕ�ȡ������
				USART3_RxStruct.Flag = 1;
            }
			else
			{
				Slave_State[Slave_number - 1] = USART3_RxStruct.Buff[2];
                ReadSpeed1[Slave_number - 1] = USART3_RxStruct.Buff[3];
                ReadSpeed2[Slave_number - 1] = USART3_RxStruct.Buff[4];
				
				USART3_RxStruct.Buff[2] = 0;
				USART3_RxStruct.Buff[3] = 0;
				USART3_RxStruct.Buff[4] = 0;
			}
			
        }
        memset(USART3_RxStruct.Buff, 0, USART3_RxStruct.Rx_len);
        /////////////////////////////////////

        HAL_UART_Receive_DMA(SlaveUART, USART3_RxStruct.Rx_Buff, 200);           //������ʼDMA����
    }
}


/////////////////////////////////////////////
void TaskSendSpeed(uint8_t speed)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        Slave_number = i + 1;
        taskENTER_CRITICAL();
        SendSlaveSpeed(Slave_number, speed,0xc1);
        taskEXIT_CRITICAL();
//        osDelay(1);

        taskENTER_CRITICAL();
        SendSlaveReadCMD(Slave_number);
        taskEXIT_CRITICAL();
//        osDelay(100);
		while(1)
		{
			osDelay(1);
			static uint8_t t=0;
			if(USART3_RxStruct.Flag == 1)
			{
				USART3_RxStruct.Flag = 0;
				
				
				break;
			}
			else
			{
				//��ʱ����t ms��û�����ݷ�����������
				t++;
				if(t==100)
				{
					t=0;
					break;
				}
			}
		}
    }
}

void TaskSendRead()
{
    for (uint8_t i = 0; i < 4; i++)
    {
        Slave_number = i + 1;
        taskENTER_CRITICAL();
        SendSlaveReadCMD(Slave_number);
        taskEXIT_CRITICAL();
		while(1)
		{
			osDelay(1);
			static uint8_t t=0;
			if(USART3_RxStruct.Flag == 1)
			{
				USART3_RxStruct.Flag = 0;
				
				break;
			}
			else
			{
				//��ʱ����t ms��û�����ݷ�����������
				t++;
				if(t==100)
				{
					t=0;
					break;
				}
			}
		}
    }
}

//�����ٶ���Ϣz
void SendSlaveSpeed(uint8_t slace_add, uint8_t speed,uint8_t dir)
{
    static uint8_t Tx_CMD[10] = { 0 };
    Tx_CMD[0] = 0xEB;
    Tx_CMD[1] = slace_add;
    Tx_CMD[2] = dir;
    Tx_CMD[3] = speed;
    Tx_CMD[4] = 0x90;
	HAL_GPIO_WritePin(SlaveRS485_RE_GPIO_Port,SlaveRS485_RE_Pin,GPIO_PIN_SET);
    HAL_UART_Transmit(SlaveUART, Tx_CMD, 5,500);
	HAL_GPIO_WritePin(SlaveRS485_RE_GPIO_Port,SlaveRS485_RE_Pin,GPIO_PIN_RESET);
}


/*
1  8     14 15
4  2     13 12

2  4     12
8  1
*/

//���Ͷ��ӻ������������״̬������ٶ���Ϣ��
void SendSlaveReadCMD(uint8_t slace_add)
{
    static uint8_t Tx_CMD[10] = { 0 };
    Tx_CMD[0] = 0xEB;
    Tx_CMD[1] = slace_add;
    Tx_CMD[2] = 0xd1;
    Tx_CMD[3] = 0x90;
	HAL_GPIO_WritePin(SlaveRS485_RE_GPIO_Port,SlaveRS485_RE_Pin,GPIO_PIN_SET);
    HAL_UART_Transmit(SlaveUART, Tx_CMD, 4,500);
	HAL_GPIO_WritePin(SlaveRS485_RE_GPIO_Port,SlaveRS485_RE_Pin,GPIO_PIN_RESET);
}
////////////////////////////////////////////////









