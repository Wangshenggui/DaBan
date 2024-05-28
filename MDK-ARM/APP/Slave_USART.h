#ifndef _SLACE_USART_H
#define _SLACE_USART_H

#include "main.h"
#include "usart.h"
#include <stdbool.h>


#define SlaveUART           &huart3
#define SlaveUART_DMA_RX    &hdma_usart3_rx




#define Rx_LENG 200

extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx;

//种子仓数量
#define SlaveNum 8

extern uint8_t Slave_number;
extern uint8_t Slave_State[SlaveNum];

extern uint8_t ReadSpeed1[SlaveNum],ReadSpeed2[SlaveNum];

typedef struct
{
	uint8_t Flag;
    uint8_t Rx_Buff[Rx_LENG];
    uint8_t Rx_len;
    uint8_t Buff[Rx_LENG];
}USART3_RxStructure;
extern USART3_RxStructure USART3_RxStruct;

void USART3_IDLE_Handler(void);

void TaskSendSpeed(uint8_t speed);
void TaskSendRead(void);
void SendSlaveSpeed(uint8_t slace_add,uint8_t speed,uint8_t dir);
void SendSlaveReadCMD(uint8_t slace_add);



#endif
