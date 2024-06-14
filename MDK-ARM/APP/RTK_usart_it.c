#include "RTK_usart_it.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Slave_USART.h"
#include "UpperCom_USART.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"


USART4_RxStructure USART4_RxStruct;

extern //使用非阻塞发送时要将缓冲区定义为全局变量
uint8_t UpperComTxBuf[100];

extern float RTK_Speed;

extern double RTK_Longitude;
extern double RTK_Latitude;

extern double RTK_CourseAngle;

extern osSemaphoreId UARTControlBinSemHandle;
extern osSemaphoreId RecCoordBinSemHandle;

//标记RTK
uint8_t RTK_Signal=0;


//发送数据到G070-RTK上
void SendG070_RTK_Data()
{
    uint8_t str[100];
    sprintf((char*)str,"$SLAVE,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
        Slave_State[0],Slave_State[1],Slave_State[2],Slave_State[3],Slave_State[4],Slave_State[5],Slave_State[6],Slave_State[7],
        ReadSpeed1[0],ReadSpeed1[1],ReadSpeed1[2],ReadSpeed1[3],ReadSpeed1[4],ReadSpeed1[5],ReadSpeed1[6],ReadSpeed1[7],
        ReadSpeed2[0],ReadSpeed2[1],ReadSpeed2[2],ReadSpeed2[3],ReadSpeed2[4],ReadSpeed2[5],ReadSpeed2[6],ReadSpeed2[7]
    );
    
    HAL_UART_Transmit(&huart4, str, strlen((char*)str),1000);
}

//接收中断
void USART4_IDLE_Handler()
{
    if (RESET != __HAL_UART_GET_FLAG(RTK_UART, UART_FLAG_IDLE))   //判断是否是空闲中断
    {
        __HAL_UART_CLEAR_IDLEFLAG(RTK_UART);                     //清除空闲中断标志（否则会一直不断进入中断）
        HAL_UART_DMAStop(RTK_UART);

        ///////////////////////
        USART4_RxStruct.Rx_len = Rx_LENG - __HAL_DMA_GET_COUNTER(RTK_UART_DMA_RX);

        //复制到DMA传输
//        memcpy(UpperComTxBuf,USART4_RxStruct.Rx_Buff,USART4_RxStruct.Rx_len);
        memset(USART4_RxStruct.Buff,0,200);
        memcpy(USART4_RxStruct.Buff,USART4_RxStruct.Rx_Buff,USART4_RxStruct.Rx_len);
        
        
        //是查询信息
        if(UpperComTxBuf[0]=='+')
        {
            //开启DMA传输
            HAL_UART_Transmit_DMA(&huart1,UpperComTxBuf,USART4_RxStruct.Rx_len);
        }
        else if(UpperComTxBuf[0]==0xac && UpperComTxBuf[3]==0x90)
        {
            //开启DMA传输
            HAL_UART_Transmit_DMA(&huart1,UpperComTxBuf,USART4_RxStruct.Rx_len);
        }
        //RTK信息
        else if(USART4_RxStruct.Buff[0]==0xab 
            && USART4_RxStruct.Buff[USART4_RxStruct.Rx_len-2]==0xeb 
        && USART4_RxStruct.Buff[USART4_RxStruct.Rx_len-1]==0x90)
        {
            //表示有RTK信号
            RTK_Signal=1;
            
            //时间
//            UpperComTxBuf[29] = USART4_RxStruct.Buff[1];
//            UpperComTxBuf[30] = USART4_RxStruct.Buff[2];
//            UpperComTxBuf[31] = USART4_RxStruct.Buff[3];
            
            #include "EdgeProcessing.h"
            
            UpperComTxBuf[29] = LocationJudging_Struct.Flag[0];
            UpperComTxBuf[30] = LocationJudging_Struct.Flag[1];
            UpperComTxBuf[31] = LocationJudging_Struct.Flag[3];
            
            //日期
            UpperComTxBuf[34] = USART4_RxStruct.Buff[21];
            UpperComTxBuf[35] = USART4_RxStruct.Buff[22];
            UpperComTxBuf[36] = USART4_RxStruct.Buff[23];
            
            //速度
            union
            {
                double v;
                unsigned char bit[8];
            }RTK_s;
            RTK_s.bit[0]=USART4_RxStruct.Buff[5];
            RTK_s.bit[1]=USART4_RxStruct.Buff[6];
            RTK_s.bit[2]=USART4_RxStruct.Buff[7];
            RTK_s.bit[3]=USART4_RxStruct.Buff[8];
            RTK_s.bit[4]=USART4_RxStruct.Buff[9];
            RTK_s.bit[5]=USART4_RxStruct.Buff[10];
            RTK_s.bit[6]=USART4_RxStruct.Buff[11];
            RTK_s.bit[7]=USART4_RxStruct.Buff[12];
            RTK_Speed=RTK_s.v;
            
            
            //经度
            union
            {
                double v;
                unsigned char bit[8];
            }Longitude;
            Longitude.bit[0]= USART4_RxStruct.Buff[25];
            Longitude.bit[1]= USART4_RxStruct.Buff[26];
            Longitude.bit[2]= USART4_RxStruct.Buff[27];
            Longitude.bit[3]= USART4_RxStruct.Buff[28];
            Longitude.bit[4]= USART4_RxStruct.Buff[29];
            Longitude.bit[5]= USART4_RxStruct.Buff[30];
            Longitude.bit[6]= USART4_RxStruct.Buff[31];
            Longitude.bit[7]= USART4_RxStruct.Buff[32];
            RTK_Longitude = Longitude.v;
            
            //纬度
            union
            {
                double v;
                unsigned char bit[8];
            }Latitude;
            Latitude.bit[0]= USART4_RxStruct.Buff[33];
            Latitude.bit[1]= USART4_RxStruct.Buff[34];
            Latitude.bit[2]= USART4_RxStruct.Buff[35];
            Latitude.bit[3]= USART4_RxStruct.Buff[36];
            Latitude.bit[4]= USART4_RxStruct.Buff[37];
            Latitude.bit[5]= USART4_RxStruct.Buff[38];
            Latitude.bit[6]= USART4_RxStruct.Buff[39];
            Latitude.bit[7]= USART4_RxStruct.Buff[40];
            RTK_Latitude = Latitude.v;
            
            //航向角
            union
            {
                double v;
                unsigned char bit[8];
            }GPS_CourseAngle;
            GPS_CourseAngle.bit[0]= USART4_RxStruct.Buff[13];
            GPS_CourseAngle.bit[1]= USART4_RxStruct.Buff[14];
            GPS_CourseAngle.bit[2]= USART4_RxStruct.Buff[15];
            GPS_CourseAngle.bit[3]= USART4_RxStruct.Buff[16];
            GPS_CourseAngle.bit[4]= USART4_RxStruct.Buff[17];
            GPS_CourseAngle.bit[5]= USART4_RxStruct.Buff[18];
            GPS_CourseAngle.bit[6]= USART4_RxStruct.Buff[19];
            GPS_CourseAngle.bit[7]= USART4_RxStruct.Buff[20];
            RTK_CourseAngle = GPS_CourseAngle.v;
            
            //定位模式
            UpperComTxBuf[53] = USART4_RxStruct.Buff[24];
        }
        else if(USART4_RxStruct.Buff[0]==0xeb && USART4_RxStruct.Buff[USART4_RxStruct.Rx_len-1]==0x90)
        {
            if(USART4_RxStruct.Buff[1]==99 && 
                USART4_RxStruct.Buff[2]==99 && 
            USART4_RxStruct.Buff[3]==99 && 
            USART4_RxStruct.Buff[4]==99)
            {
//                WRMapTest();
                osSemaphoreRelease(RecCoordBinSemHandle);//释放记录坐标信号量
            }
            else
            {
                //记录控制信息
                UARTControl_Stru.ControlSpeed = USART4_RxStruct.Buff[3];
                UARTControl_Stru.ButBit = USART4_RxStruct.Buff[1];
                UARTControl_Stru.Dir = USART4_RxStruct.Buff[2];
                
                //当水泵关闭时有效
                if (RunGuidance_Struct.SysBeng == 0)
                {
                    HAL_GPIO_TogglePin(RTK_LED_GPIO_Port,RTK_LED_Pin);
                    osSemaphoreRelease(UARTControlBinSemHandle);//释放控制信号量
                }
            }
        }
        
        memset(USART4_RxStruct.Rx_Buff,0,200);
        HAL_UART_Receive_DMA(RTK_UART, USART4_RxStruct.Rx_Buff, 200);           //重启开始DMA传输
    }
}












