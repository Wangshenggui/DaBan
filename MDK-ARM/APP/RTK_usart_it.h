#ifndef _RTK_USART_IT_H
#define _RTK_USART_IT_H

#include "usart.h"


#define RTK_UART           &huart4
#define RTK_UART_DMA_RX    &hdma_uart4_rx


#define Rx_LENG 200

typedef struct
{
    uint8_t Rx_Buff[Rx_LENG];
    uint8_t Rx_len;
    uint8_t Buff[Rx_LENG];
}USART4_RxStructure;
extern USART4_RxStructure USART4_RxStruct;

void SendG070_RTK_Data(void);
void USART4_IDLE_Handler(void);


#endif
