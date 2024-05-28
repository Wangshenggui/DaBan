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


//ʹ�÷���������ʱҪ������������Ϊȫ�ֱ���
uint8_t UpperComTxBuf[100] = { 0 };

UARTControl_Structure UARTControl_Stru;

//����ʱ��
uint16_t RunTime = 0;

float RTK_Speed = 0.0f;
double RTK_Longitude = 0.0f;
double RTK_Latitude = 0.0f;
double RTK_LongitudeH = 0.0f;
double RTK_LatitudeH = 0.0f;
//�����
double RTK_CourseAngle = 0.0f;

extern//���RTK
uint8_t RTK_Signal;


extern osSemaphoreId UpperComBinSemHandle;
extern osSemaphoreId UARTControlBinSemHandle;

extern osTimerId UpperRTK_TimerHandle;

//����λ���������Ӳ�����\
�����ϱ���ʽ��\
 0          1                                2                                 3               4            5\
EB   ���Ӳ�(1-8)״̬   ���Ӳ�(1-8)����ٶ�(һ���������������16������)   GPS�ٶ�(4���ֽ�)   ����״̬(���ַ�)   90\
            0\
0----->>>����֡ͷ\
           1    2    3    4    5    6    7    8\
1----->>>dat1 dat2 dat3 dat4 dat5 dat6 dat7 dat8\
   ʾ����(00   01   01   00   01   01   00   01)\
           9   10    11  12    13  14   15   16   17    18    19    20    21    22    23    24\
2----->>>dat1 dat2 dat3 dat4 dat5 dat6 dat7 dat8 dat9 dat10 dat11 dat10 dat13 dat14 dat15 dat16\
         ��S1 ��S2 ��S3 ��S4 ��S5 ��S6 ��S7  ��S8 ��S1 ��S2  ��S3   ��S4  ��S5  ��S6  ��S7  ��S8\
           25  26    27   28\
3----->>>dat1 dat2 dat3 dat4\
            GPS�ٶ�\
           29 30\
4----->>>�ַ���"OK"\
            31\
5----->>>����֡β
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
        //GPSʱ��
        UpperComTxBuf[29] = GPS_Struct.Hour;
        UpperComTxBuf[30] = GPS_Struct.Min;
        UpperComTxBuf[31] = GPS_Struct.Sec;
    }


    //����ʱ��
    UpperComTxBuf[32] = RunTime >> 8;
    UpperComTxBuf[33] = (uint8_t)RunTime;

    if(RTK_Signal==0)
    {
        //GPS����
        UpperComTxBuf[34] = GPS_Struct.Year;
        UpperComTxBuf[35] = GPS_Struct.Month;
        UpperComTxBuf[36] = GPS_Struct.Day;
    }
    
    //����
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
    
    //γ��
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
    
    //��λģʽ
    //53
    if(RTK_Signal==0)
    {
        UpperComTxBuf[53] = GPS_Struct.ModeIndicator;
    }
    
    
    //����
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
    
    //γ��
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

//���͵�RTK��
uint8_t RTK_SendBuff[200];
//ת��������
static uint8_t str[200];


//�ϴ���־λ
uint8_t UpFlag=0;

//�����ж�
void UART1_IDLE_Handler()
{
    if (RESET != __HAL_UART_GET_FLAG(UpperUART, UART_FLAG_IDLE))   //�ж��Ƿ��ǿ����ж�
    {
        __HAL_UART_CLEAR_IDLEFLAG(UpperUART);                     //��������жϱ�־�������һֱ���Ͻ����жϣ�
        HAL_UART_DMAStop(UpperUART);

        ///////////////////////
        UART1_RxStruct.Rx_len = Rx_LENG - __HAL_DMA_GET_COUNTER(UpperUART_DMA_RX);

        memset(RTK_SendBuff, 0, 200);
        //����DMA���仺�棬��ֹDMA������������ݱ��۸�
        memcpy(RTK_SendBuff, UART1_RxStruct.Rx_Buff, UART1_RxStruct.Rx_len);


        //�ٶȿ���
        if (UART1_RxStruct.Rx_Buff[0] == 0xeb
            && (UART1_RxStruct.Rx_Buff[2] == 0xc1 || UART1_RxStruct.Rx_Buff[2] == 0xc2)
            && UART1_RxStruct.Rx_Buff[4] == 0x90)
        {
            //��¼������Ϣ
            UARTControl_Stru.ControlSpeed = UART1_RxStruct.Rx_Buff[3];
            UARTControl_Stru.ButBit = UART1_RxStruct.Rx_Buff[1];
            UARTControl_Stru.Dir = UART1_RxStruct.Rx_Buff[2];

            //��ˮ�ùر�ʱ��Ч
            if (RunGuidance_Struct.SysBeng == 0)
            {
                osSemaphoreRelease(UARTControlBinSemHandle);//�ͷſ����ź���
            }
        }
        //����λ����Ӧ
        else if (UART1_RxStruct.Rx_Buff[0] == 'R')
        {
            UpFlag=1;
//            osSemaphoreRelease(UpperComBinSemHandle);//�ͷ��ϴ��ź���
        }
        //�ر���λ����Ӧ
        else if (UART1_RxStruct.Rx_Buff[0] == 'E')
        {
            UpFlag=0;
        }
        //ATָ��
        else if (UART1_RxStruct.Rx_Buff[0] == 'A'
            && UART1_RxStruct.Rx_Buff[1] == 'T')
        {
            memset(str, 0, 200);
            sprintf((char*)str, "%s4G", RTK_SendBuff);
            HAL_UART_Transmit_DMA(&huart4, str, strlen((char*)str));
            //��������RTK������
            osTimerStart(UpperRTK_TimerHandle, 500);//1000����ص������ص�����
        }
        //GPxxx����
        else if (UART1_RxStruct.Rx_Buff[0] == 'G'
            && UART1_RxStruct.Rx_Buff[1] == 'P')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //��������RTK������
            osTimerStart(UpperRTK_TimerHandle, 500);//1000����ص������ص�����
        }
        //������������
        else if (UART1_RxStruct.Rx_Buff[0] == 's'
            && UART1_RxStruct.Rx_Buff[1] == 'a')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //��������RTK������
            osTimerStart(UpperRTK_TimerHandle, 500);//1000����ص������ص�����
        }
        //�ָ���������
        else if (UART1_RxStruct.Rx_Buff[0] == 'f'
            && UART1_RxStruct.Rx_Buff[1] == 'r')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //��������RTK������
            osTimerStart(UpperRTK_TimerHandle, 500);//1000����ص������ص�����
        }
        //����COM1������
        else if (UART1_RxStruct.Rx_Buff[0] == 'c'
            && UART1_RxStruct.Rx_Buff[1] == 'o'
            && UART1_RxStruct.Rx_Buff[2] == 'n')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //��������RTK������
            osTimerStart(UpperRTK_TimerHandle, 500);//1000����ص������ص�����
        }
        //����COM1����
        else if (UART1_RxStruct.Rx_Buff[0] == 'u'
            && UART1_RxStruct.Rx_Buff[1] == 'n'
            && UART1_RxStruct.Rx_Buff[2] == 'l')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //��������RTK������
            osTimerStart(UpperRTK_TimerHandle, 500);//1000����ص������ص�����
        }
        //��������
        else if (UART1_RxStruct.Rx_Buff[0] == 'm'
            && UART1_RxStruct.Rx_Buff[1] == 'a'
            && UART1_RxStruct.Rx_Buff[2] == 's')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //��������RTK������
            osTimerStart(UpperRTK_TimerHandle, 500);//1000����ص������ص�����
        }
        //��������
        else if (UART1_RxStruct.Rx_Buff[0] == 'u'
            && UART1_RxStruct.Rx_Buff[1] == 'n'
            && UART1_RxStruct.Rx_Buff[2] == 'm')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //��������RTK������
            osTimerStart(UpperRTK_TimerHandle, 500);//1000����ص������ص�����
        }
        //���ô�����
        else if (UART1_RxStruct.Rx_Buff[0] == 'C'
            && UART1_RxStruct.Rx_Buff[1] == 'O'
            && UART1_RxStruct.Rx_Buff[2] == 'N')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //��������RTK������
            osTimerStart(UpperRTK_TimerHandle, 500);//1000����ص������ص�����
        }
        //����/���� ������     �����߽���͹���ģʽ
        else if (UART1_RxStruct.Rx_Buff[0] == 'C'
            && UART1_RxStruct.Rx_Buff[1] == 'O'
            && UART1_RxStruct.Rx_Buff[2] == 'N')
        {
            HAL_UART_Transmit_DMA(&huart4, RTK_SendBuff, UART1_RxStruct.Rx_len);
            //��������RTK������
            osTimerStart(UpperRTK_TimerHandle, 500);//1000����ص������ص�����
        }

        //        HAL_UART_Transmit_DMA(UpperUART,RTK_SendBuff,UART1_RxStruct.Rx_len);


        memcpy(UART1_RxStruct.Buff, UART1_RxStruct.Rx_Buff, 200);

        memset(UART1_RxStruct.Rx_Buff, 0, Rx_LENG);
        /////////////////////////////////////

        memset(UART1_RxStruct.Buff, 0, Rx_LENG);
        /////////////////////////////////////

        HAL_UART_Receive_DMA(UpperUART, UART1_RxStruct.Rx_Buff, 200);           //������ʼDMA����
    }
}








