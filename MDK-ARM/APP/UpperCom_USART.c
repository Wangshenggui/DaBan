#include "UpperCom_USART.h"
#include "Slave_USART.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gps.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

//#include "tim.h"



UART1_RxStructure UART1_RxStruct;


//使用非阻塞发送时要将缓冲区定义为全局变量
uint8_t UpperComTxBuf[100] = { 0 };

UARTControl_Structure UARTControl_Stru;

//工作时长
uint16_t RunTime = 0;

float RTK_Speed = 0.0f;
double RTK_Longitude = 0.0f;
double RTK_Latitude = 0.0f;
double RTK_LongitudeH = 0.0f;
double RTK_LatitudeH = 0.0f;
//航向角
double RTK_CourseAngle = 0.0f;

extern//标记RTK
uint8_t RTK_Signal;


extern osSemaphoreId UpperComBinSemHandle;
extern osSemaphoreId UARTControlBinSemHandle;

extern osTimerId UpperRTK_TimerHandle;

//向上位机更新种子仓数据\
数据上报格式：\
 0          1                                2                                 3               4            5\
EB   种子仓(1-8)状态   种子仓(1-8)电机速度(一个仓两个电机，共16组数据)   GPS速度(4个字节)   连接状态(两字符)   90\
            0\
0----->>>数据帧头\
           1    2    3    4    5    6    7    8\
1----->>>dat1 dat2 dat3 dat4 dat5 dat6 dat7 dat8\
   示例：(00   01   01   00   01   01   00   01)\
           9   10    11  12    13  14   15   16   17    18    19    20    21    22    23    24\
2----->>>dat1 dat2 dat3 dat4 dat5 dat6 dat7 dat8 dat9 dat10 dat11 dat10 dat13 dat14 dat15 dat16\
         主S1 主S2 主S3 主S4 主S5 主S6 主S7  主S8 辅S1 辅S2  辅S3   辅S4  辅S5  辅S6  辅S7  辅S8\
           25  26    27   28\
3----->>>dat1 dat2 dat3 dat4\
            GPS速度\
           29 30\
4----->>>字符串"OK"\
            31\
5----->>>数据帧尾
void SendSlaveUpperComData(void)
{
    static uint8_t i = 0;

    UpperComTxBuf[0] = 0xEB;
    for (i = 1; i <= 8; i++)
    {
        UpperComTxBuf[i] = Slave_State[i - 1];
        Slave_State[i - 1] = 0;
    }
    for (i = 9; i <= 16; i++)
    {
        UpperComTxBuf[i] = ReadSpeed1[i - 9];
        ReadSpeed1[i - 9] = 0;
    }
    for (i = 17; i <= 24; i++)
    {
        UpperComTxBuf[i] = ReadSpeed2[i - 17];
        ReadSpeed2[i - 17] = 0;
    }
    union
    {
        float v;
        unsigned char bit[4];
    }GPS_Speed;
    if(RTK_Signal==0)
    {
        GPS_Speed.v = GPS_Struct.Speed;
    }
    else
    {
        GPS_Speed.v = (float)RTK_Speed;
    }
    UpperComTxBuf[25] = GPS_Speed.bit[0];
    UpperComTxBuf[26] = GPS_Speed.bit[1];
    UpperComTxBuf[27] = GPS_Speed.bit[2];
    UpperComTxBuf[28] = GPS_Speed.bit[3];

    if(RTK_Signal==0)
    {
        //GPS时间
        UpperComTxBuf[29] = GPS_Struct.Hour;
        UpperComTxBuf[30] = GPS_Struct.Min;
        UpperComTxBuf[31] = GPS_Struct.Sec;
    }


    //运行时长
    UpperComTxBuf[32] = RunTime >> 8;
    UpperComTxBuf[33] = (uint8_t)RunTime;

    if(RTK_Signal==0)
    {
        //GPS日期
        UpperComTxBuf[34] = GPS_Struct.Year;
        UpperComTxBuf[35] = GPS_Struct.Month;
        UpperComTxBuf[36] = GPS_Struct.Day;
    }
    
    //经度
    union
    {
        double v;
        unsigned char bit[8];
    }Longitude;
    if(RTK_Signal==0)
    {
        Longitude.v = GPS_Struct.LongitudeDegrees;
    }
    else
    {
        Longitude.v = RTK_Longitude;
    }
    UpperComTxBuf[37] = Longitude.bit[0];
    UpperComTxBuf[38] = Longitude.bit[1];
    UpperComTxBuf[39] = Longitude.bit[2];
    UpperComTxBuf[40] = Longitude.bit[3];
    UpperComTxBuf[41] = Longitude.bit[4];
    UpperComTxBuf[42] = Longitude.bit[5];
    UpperComTxBuf[43] = Longitude.bit[6];
    UpperComTxBuf[44] = Longitude.bit[7];
    
    //纬度
    union
    {
        double v;
        unsigned char bit[8];
    }Latitude;
    if(RTK_Signal==0)
    {
        Latitude.v = GPS_Struct.LatitudeDegrees;
    }
    else
    {
        Latitude.v = RTK_Latitude;
    }
    UpperComTxBuf[45] = Latitude.bit[0];
    UpperComTxBuf[46] = Latitude.bit[1];
    UpperComTxBuf[47] = Latitude.bit[2];
    UpperComTxBuf[48] = Latitude.bit[3];
    UpperComTxBuf[49] = Latitude.bit[4];
    UpperComTxBuf[50] = Latitude.bit[5];
    UpperComTxBuf[51] = Latitude.bit[6];
    UpperComTxBuf[52] = Latitude.bit[7];
    
    //定位模式
    //53
    if(RTK_Signal==0)
    {
        UpperComTxBuf[53] = GPS_Struct.ModeIndicator;
    }
    
    
    //经度
    if(RTK_Signal==0)
    {
        Longitude.v = GPS_Struct.LongitudeDegrees;
    }
    else
    {
        Longitude.v = RTK_LongitudeH;
    }
    UpperComTxBuf[54] = Longitude.bit[0];
    UpperComTxBuf[55] = Longitude.bit[1];
    UpperComTxBuf[56] = Longitude.bit[2];
    UpperComTxBuf[57] = Longitude.bit[3];
    UpperComTxBuf[58] = Longitude.bit[4];
    UpperComTxBuf[59] = Longitude.bit[5];
    UpperComTxBuf[60] = Longitude.bit[6];
    UpperComTxBuf[61] = Longitude.bit[7];
    
    //纬度
    if(RTK_Signal==0)
    {
        Latitude.v = GPS_Struct.LatitudeDegrees;
    }
    else
    {
        Latitude.v = RTK_LatitudeH;
    }
    UpperComTxBuf[62] = Latitude.bit[0];
    UpperComTxBuf[63] = Latitude.bit[1];
    UpperComTxBuf[64] = Latitude.bit[2];
    UpperComTxBuf[65] = Latitude.bit[3];
    UpperComTxBuf[66] = Latitude.bit[4];
    UpperComTxBuf[67] = Latitude.bit[5];
    UpperComTxBuf[68] = Latitude.bit[6];
    UpperComTxBuf[69] = Latitude.bit[7];


    UpperComTxBuf[70] = 'O';
    UpperComTxBuf[71] = 'K';
    UpperComTxBuf[72] = 0x90;

    HAL_UART_Transmit(UpperUART, UpperComTxBuf, 73, 1099);
}

//发送到RTK板
uint8_t RTK_SendBuff[200];
//转换缓冲区
static uint8_t str[200];


//上传标志位
uint8_t UpFlag=0;

//接收中断
void UART1_IDLE_Handler()
{
    if (RESET != __HAL_UART_GET_FLAG(UpperUART, UART_FLAG_IDLE))   //判断是否是空闲中断
    {
        __HAL_UART_CLEAR_IDLEFLAG(UpperUART);                     //清除空闲中断标志（否则会一直不断进入中断）
        HAL_UART_DMAStop(UpperUART);

        ///////////////////////
        UART1_RxStruct.Rx_len = Rx_LENG - __HAL_DMA_GET_COUNTER(UpperUART_DMA_RX);

        memset(RTK_SendBuff, 0, 200);
        //用于DMA传输缓存，防止DMA传输过程中数据被篡改
        memcpy(RTK_SendBuff, UART1_RxStruct.Rx_Buff, UART1_RxStruct.Rx_len);


        //速度控制
        if (UART1_RxStruct.Rx_Buff[0] == 0xeb
            && (UART1_RxStruct.Rx_Buff[2] == 0xc1 || UART1_RxStruct.Rx_Buff[2] == 0xc2)
            && UART1_RxStruct.Rx_Buff[4] == 0x90)
        {
            //记录控制信息
            UARTControl_Stru.ControlSpeed = UART1_RxStruct.Rx_Buff[3];
            UARTControl_Stru.ButBit = UART1_RxStruct.Rx_Buff[1];
            UARTControl_Stru.Dir = UART1_RxStruct.Rx_Buff[2];

            //当水泵关闭时有效
            if (RunGuidance_Struct.SysBeng == 0)
            {
                osSemaphoreRelease(UARTControlBinSemHandle);//释放控制信号量
            }
        }
        //向上位机回应
        else if (UART1_RxStruct.Rx_Buff[0] == 'R')
        {
            UpFlag=1;
//            osSemaphoreRelease(UpperComBinSemHandle);//释放上传信号量
        }
        //关闭上位机回应
        else if (UART1_RxStruct.Rx_Buff[0] == 'E')
        {
            UpFlag=0;
        }
        //AT指令
        else if (UART1_RxStruct.Rx_Buff[0] == 'A'
            && UART1_RxStruct.Rx_Buff[1] == 'T')
        {
            memset(str, 0, 200);
            sprintf((char*)str, "%s4G", RTK_SendBuff);
            HAL_UART_Transmit_DMA(&huart4, str, strlen((char*)str));
            //开启更新RTK计数器
            osTimerStart(UpperRTK_TimerHandle, 500);//1000代表回调函数回调周期
        }
        //GPxxx配置
        else if (UART1_RxStruct.Rx_Buff[0] == 'G'
            && UART1_RxStruct.Rx_Buff[1] == 'P')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //开启更新RTK计数器
            osTimerStart(UpperRTK_TimerHandle, 500);//1000代表回调函数回调周期
        }
        //保存设置命令
        else if (UART1_RxStruct.Rx_Buff[0] == 's'
            && UART1_RxStruct.Rx_Buff[1] == 'a')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //开启更新RTK计数器
            osTimerStart(UpperRTK_TimerHandle, 500);//1000代表回调函数回调周期
        }
        //恢复设置命令
        else if (UART1_RxStruct.Rx_Buff[0] == 'f'
            && UART1_RxStruct.Rx_Buff[1] == 'r')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //开启更新RTK计数器
            osTimerStart(UpperRTK_TimerHandle, 500);//1000代表回调函数回调周期
        }
        //配置COM1波特率
        else if (UART1_RxStruct.Rx_Buff[0] == 'c'
            && UART1_RxStruct.Rx_Buff[1] == 'o'
            && UART1_RxStruct.Rx_Buff[2] == 'n')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //开启更新RTK计数器
            osTimerStart(UpperRTK_TimerHandle, 500);//1000代表回调函数回调周期
        }
        //禁用COM1命令
        else if (UART1_RxStruct.Rx_Buff[0] == 'u'
            && UART1_RxStruct.Rx_Buff[1] == 'n'
            && UART1_RxStruct.Rx_Buff[2] == 'l')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //开启更新RTK计数器
            osTimerStart(UpperRTK_TimerHandle, 500);//1000代表回调函数回调周期
        }
        //禁用卫星
        else if (UART1_RxStruct.Rx_Buff[0] == 'm'
            && UART1_RxStruct.Rx_Buff[1] == 'a'
            && UART1_RxStruct.Rx_Buff[2] == 's')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //开启更新RTK计数器
            osTimerStart(UpperRTK_TimerHandle, 500);//1000代表回调函数回调周期
        }
        //启用卫星
        else if (UART1_RxStruct.Rx_Buff[0] == 'u'
            && UART1_RxStruct.Rx_Buff[1] == 'n'
            && UART1_RxStruct.Rx_Buff[2] == 'm')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //开启更新RTK计数器
            osTimerStart(UpperRTK_TimerHandle, 500);//1000代表回调函数回调周期
        }
        //启用从天线
        else if (UART1_RxStruct.Rx_Buff[0] == 'C'
            && UART1_RxStruct.Rx_Buff[1] == 'O'
            && UART1_RxStruct.Rx_Buff[2] == 'N')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //开启更新RTK计数器
            osTimerStart(UpperRTK_TimerHandle, 500);//1000代表回调函数回调周期
        }
        //启用/禁用 从天线     从天线进入低功耗模式
        else if (UART1_RxStruct.Rx_Buff[0] == 'C'
            && UART1_RxStruct.Rx_Buff[1] == 'O'
            && UART1_RxStruct.Rx_Buff[2] == 'N')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //开启更新RTK计数器
            osTimerStart(UpperRTK_TimerHandle, 500);//1000代表回调函数回调周期
        }

        //        HAL_UART_Transmit_DMA(UpperUART,RTK_SendBuff,UART1_RxStruct.Rx_len);


        memcpy(UART1_RxStruct.Buff, UART1_RxStruct.Rx_Buff, 200);

        memset(UART1_RxStruct.Rx_Buff, 0, Rx_LENG);
        /////////////////////////////////////

        memset(UART1_RxStruct.Buff, 0, Rx_LENG);
        /////////////////////////////////////

        HAL_UART_Receive_DMA(UpperUART, UART1_RxStruct.Rx_Buff, 200);           //重启开始DMA传输
    }
}








