#ifndef _UPPER_USART_H
#define _UPPER_USART_H

#include "main.h"
#include "usart.h"


#define UpperUART           &huart1
#define UpperUART_DMA_RX    &hdma_usart1_rx

extern //使用非阻塞发送时要将缓冲区定义为全局变量
uint8_t UpperComTxBuf[100];

extern //工作时长
uint16_t RunTime;

void SendSlaveUpperComData(void);

#define Rx_LENG 200

typedef struct
{
    uint8_t Rx_Buff[Rx_LENG];
    uint8_t Rx_len;
    uint8_t Buff[Rx_LENG];
}UART1_RxStructure;
extern UART1_RxStructure UART1_RxStruct;

typedef struct
{
    uint8_t ButBit;
    uint8_t ControlSpeed;
    uint8_t Dir;
}UARTControl_Structure;
extern UARTControl_Structure UARTControl_Stru;


void SendSlaveUpperComData(void);
void UART1_IDLE_Handler(void);




#endif
