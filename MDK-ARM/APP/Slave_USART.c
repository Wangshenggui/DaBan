#include "Slave_USART.h"
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"


USART3_RxStructure USART3_RxStruct;

//从机编号
uint8_t Slave_number = 0x00;
uint8_t Slave_State[SlaveNum] = { 0 };
//读取到的速度
uint8_t ReadSpeed1[SlaveNum] = { 0 }, ReadSpeed2[SlaveNum] = { 0 };


//接收中断
void USART3_IDLE_Handler()
{
    if (RESET != __HAL_UART_GET_FLAG(SlaveUART, UART_FLAG_IDLE))   //判断是否是空闲中断
    {
		
		
        __HAL_UART_CLEAR_IDLEFLAG(SlaveUART);                     //清除空闲中断标志（否则会一直不断进入中断）
        HAL_UART_DMAStop(SlaveUART);

        ///////////////////////
        //获取长度
        USART3_RxStruct.Rx_len = Rx_LENG - __HAL_DMA_GET_COUNTER(SlaveUART_DMA_RX);
        //复制字符串
		
//        memset(Slave_State,0,SlaveNum);
//        memset(ReadSpeed1,0,SlaveNum);
//        memset(ReadSpeed2,0,SlaveNum);
        memset(USART3_RxStruct.Buff, 0, USART3_RxStruct.Rx_len);
        memcpy(USART3_RxStruct.Buff, USART3_RxStruct.Rx_Buff, USART3_RxStruct.Rx_len);
        //清空缓冲区
        memset(USART3_RxStruct.Rx_Buff, 0, USART3_RxStruct.Rx_len);
        /////////////////////////////////////
        if (USART3_RxStruct.Buff[0] == 0xEB && USART3_RxStruct.Buff[5] == 0x90)
        {
            /*----------------------------------------------*/
            //从机发送的状态\
                            0     1      2    3\
            返回          ：EB 从机地址 状态  90
			Slave_State[Slave_number - 1] = 0;
			ReadSpeed1[Slave_number - 1] = 0;
			ReadSpeed2[Slave_number - 1] = 0;
            if (USART3_RxStruct.Buff[1] == Slave_number)
            {
                Slave_State[Slave_number - 1] = USART3_RxStruct.Buff[2];
                ReadSpeed1[Slave_number - 1] = USART3_RxStruct.Buff[3];
                ReadSpeed2[Slave_number - 1] = USART3_RxStruct.Buff[4];
				
				//标记读取到数据
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

        HAL_UART_Receive_DMA(SlaveUART, USART3_RxStruct.Rx_Buff, 200);           //重启开始DMA传输
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
				//超时处理（t ms内没有数据返回则跳过）
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
				//超时处理（t ms内没有数据返回则跳过）
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

//发送速度消息z
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

//发送读从机命令（返回种子状态，电机速度信息）
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









