#include "gps.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "system.h"



GPS_Structure GPS_Struct;
USART2_RxStructure USART2_RxStruct;



uint8_t sys_rst_n __attribute__((at(0x2000BF00)));
uint8_t sys_rst_flag __attribute__((at(0x2000BF10)));


void GPS_Init()
{
    /*��ʼ��GPS*/
    if (sys_rst_flag != 99)
    {
        sys_rst_flag = 99;
        sys_rst_n = 9;
    }
    
    if (sys_rst_flag == 99)
        sys_rst_n--;
    if (sys_rst_n > 0 && sys_rst_n < 10)
    {
        //�ȷ�һ������
        HAL_UART_Transmit(GPS_UART, (uint8_t*)" ", 1, 100);

        taskENTER_CRITICAL();
        HAL_UART_Transmit(GPS_UART, (uint8_t*)"$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*35\r\n", strlen("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*35\r\n"), 1000);
        taskEXIT_CRITICAL();
        osDelay(100);

        taskENTER_CRITICAL();
        HAL_UART_Transmit(GPS_UART, (uint8_t*)"$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*35\r\n", strlen("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*35\r\n"), 1000);
        taskEXIT_CRITICAL();
        osDelay(100);

        taskENTER_CRITICAL();
        HAL_UART_Transmit(GPS_UART, (uint8_t*)"$PMTK251,115200*1F\r\n", strlen("$PMTK251,115200*1F\r\n"), 1000);
        taskEXIT_CRITICAL();
        osDelay(100);

        taskENTER_CRITICAL();
        HAL_UART_Transmit(GPS_UART, (uint8_t*)"$PMTK251,115200*1F\r\n", strlen("$PMTK251,115200*1F\r\n"), 1000);
        taskEXIT_CRITICAL();
        osDelay(100);

        HAL_NVIC_SystemReset();
    }

    _USART2_UART_Init(115200);

    taskENTER_CRITICAL();
    HAL_UART_Transmit(GPS_UART, (uint8_t*)"$PMTK220,100*2F\r\n", strlen("$PMTK220,100*2F\r\n"), 1000);
    taskEXIT_CRITICAL();
    osDelay(100);

    taskENTER_CRITICAL();
    HAL_UART_Transmit(GPS_UART, (uint8_t*)"$PMTK220,100*2F\r\n", strlen("$PMTK220,100*2F\r\n"), 1000);
    taskEXIT_CRITICAL();
    osDelay(100);

    taskENTER_CRITICAL();
    HAL_UART_Transmit(GPS_UART, (uint8_t*)"$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*35\r\n", strlen("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*35\r\n"), 1000);
    taskEXIT_CRITICAL();
    osDelay(100);

    taskENTER_CRITICAL();
    HAL_UART_Transmit(GPS_UART, (uint8_t*)"$PQTXT,W,0,1*23\r\n", strlen("$PQTXT,W,0,1*23\r\n"), 1000);
    taskEXIT_CRITICAL();
    osDelay(100);taskENTER_CRITICAL();
    HAL_UART_Transmit(GPS_UART, (uint8_t*)"$PMTK220,100*2F\r\n", strlen("$PMTK220,100*2F\r\n"), 1000);
    taskEXIT_CRITICAL();
    osDelay(100);

    taskENTER_CRITICAL();
    HAL_UART_Transmit(GPS_UART, (uint8_t*)"$PMTK220,100*2F\r\n", strlen("$PMTK220,100*2F\r\n"), 1000);
    taskEXIT_CRITICAL();
    osDelay(100);

    taskENTER_CRITICAL();
    HAL_UART_Transmit(GPS_UART, (uint8_t*)"$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*35\r\n", strlen("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*35\r\n"), 1000);
    taskEXIT_CRITICAL();
    osDelay(100);

    taskENTER_CRITICAL();
    HAL_UART_Transmit(GPS_UART, (uint8_t*)"$PQTXT,W,0,1*23\r\n", strlen("$PQTXT,W,0,1*23\r\n"), 1000);
    taskEXIT_CRITICAL();
    osDelay(100);

    taskENTER_CRITICAL();
    HAL_UART_Receive_IT(GPS_UART, &USART2_RxStruct.Receive, 1);
    taskEXIT_CRITICAL();
    
    osDelay(1000);
}



//$PMTK220,200
//�������ֵ
char calculateChecksum(const char* partialSentence) 
{
    char checksum = 0;

    // Start from the character after the '$' symbol
    int i = 1;

    // XOR all characters until the '*' delimiter or end of string
    while (partialSentence[i] != '*' && partialSentence[i] != '\0') {
        checksum ^= partialSentence[i];
        i++;
    }

    return checksum;
}

// ��ȡ�ӵ�n�����ź�ʼ���ַ�
//static char getCharAfterNthComma(const char* str, int n) {
//    const char* character = str;

//    // ��λ����n������
//    for (int i = 0; i < n; i++) {
//        character = strchr(character, ',');
//        if (!character) {
//            // ������Ų���n�������ؿ��ַ�
//            return '\0';
//        }
//        // �ƶ�����һ���ַ�
//        character++;
//    }

//    // ���ص�n�����ź���ַ�
//    return *character;
//}
//�ü��ַ���
static char* substring(char* dst, char* src, int start, int len)
{
    int length = 100;//��󳤶�
    if (start >= length || start < 0)
        return NULL;
    if (len > length)
        len = length - start;
    src += start;
    while (len--)
    {
        *(dst++) = *(src++);
    }
    *(dst++) = '\0';
    return dst;
}
// ��ȡ�ӵ�n�����ź�ʼ���ַ���
static void getStringAfterNthComma(const char* string, int n, char* result, size_t max_length) {
    const char* start = string;

    int xor_sum = 0;//У��ֵ
    int i = 0;
    unsigned char len = 0;//�����ַ�������
    char checksum_str[3];

    //���㳤��
    while (string[len] != '\0')len++;
    // �������ݵ�����
    for (i = 1; i < len; i++)
    {
        if (string[i] == '*')
            break;
        xor_sum ^= string[i];
    }
    sprintf(checksum_str, "%02X", xor_sum);
    if (strncmp(checksum_str, (const char*)&string[i + 1], 2) == 0) //ͨ�� 
    {
        // ��λ����n������
        for (int i = 0; i < n; i++) {
            start = strchr(start, ',');
            if (!start) {
                // ������Ų���n�����������Ϊ���ַ���
                result[0] = '\0';
                return;
            }

            // �ƶ�����һ���ַ�
            start++;
        }

        // ���㶺�ź��ַ����ĳ���
        const char* end = strchr(start, ',');
        size_t length = end ? (size_t)(end - start) : strlen(start);

        // ��ȡ�ַ��������Ƶ����
        if (length < max_length - 1) {
            strncpy(result, start, length);
            // �ֶ���� null ��β
            result[length] = '\0';
        }
        else {
            // Ŀ�곤�Ȳ��㣬�ض��ַ���
            strncpy(result, start, max_length - 1);
            result[max_length - 1] = '\0';
        }

        switch (n)
        {
        case(1)://ʱ��
            //����ʱ��
            substring(GPS_Struct.str, result, 0, 2);
            GPS_Struct.Hour = (uint8_t)atof(GPS_Struct.str) + 8;
            substring(GPS_Struct.str, result, 2, 2);
            GPS_Struct.Min = (uint8_t)atof(GPS_Struct.str);
            substring(GPS_Struct.str, result, 4, 2);
            GPS_Struct.Sec = (uint8_t)atof(GPS_Struct.str);
            break;
        case(2)://��λ״̬
            GPS_Struct.LocationState = result[0];
            break;
        case(3)://γ��
            //����γ��
            //��
            substring(GPS_Struct.str, result, 0, 2);
            GPS_Struct.LatitudeDegrees = (uint8_t)atoi(GPS_Struct.str);
            //��
            substring(GPS_Struct.str, result, 2, 2);
            GPS_Struct.LatitudeMinutes = (uint8_t)atoi(GPS_Struct.str);
            /*GPS*/
            if (strlen(result) == 11)
            {
                //��
                substring(GPS_Struct.str, result, 5, 6);
                GPS_Struct.LatitudeSeconds = (uint8_t)(atof(GPS_Struct.str) * 0.000001 * 60 + 0.5);
            }
            else if (strlen(result) == 9)
            {
                //��
                substring(GPS_Struct.str, result, 5, 4);
                GPS_Struct.LatitudeSeconds = (uint8_t)(atof(GPS_Struct.str) * 0.0001 * 60 + 0.5);
            }
            /*RTK*/
            else if (strlen(result) == 13)
            {
                //��
                substring(GPS_Struct.str, result, 5, 8);
                GPS_Struct.LatitudeSeconds = (uint8_t)(atof(GPS_Struct.str) * 0.00000001 * 60 + 0.5);
            }
            break;
        case(4)://N/S
            GPS_Struct.LatitudeDir = result[0];
            break;
        case(5)://����
            //��������
            //��
            substring(GPS_Struct.str, result, 0, 3);
            GPS_Struct.LongitudeDegrees = (uint8_t)atoi(GPS_Struct.str);
            //��
            substring(GPS_Struct.str, result, 3, 2);
            GPS_Struct.LongitudeMinutes = (uint8_t)atoi(GPS_Struct.str);

            /*GPS*/
            if (strlen(result) == 11)
            {
                //��
                substring(GPS_Struct.str, result, 6, 6);
                GPS_Struct.LongitudeSeconds = (uint8_t)(atof(GPS_Struct.str) * 0.000001 * 60 + 0.5);
            }
            else if (strlen(result) == 9)
            {
                //��
                substring(GPS_Struct.str, result, 6, 4);
                GPS_Struct.LongitudeSeconds = (uint8_t)(atof(GPS_Struct.str) * 0.0001 * 60 + 0.5);
            }
            /*RTK*/
            else if (strlen(result) == 14)
            {
                //��
                substring(GPS_Struct.str, result, 6, 8);
                GPS_Struct.LongitudeSeconds = (uint8_t)(atof(GPS_Struct.str) * 0.00000001 * 60 + 0.5);
            }
            break;
        case(6)://E/W
            GPS_Struct.LongitudeDir = result[0];
            break;
        case(7)://�����ٶ�
            GPS_Struct.Speed = (float)atof(result);
            break;
        case(8)://�����
            GPS_Struct.CourseAngle = (float)atof(result);
            break;
        case(9)://����
            //��������
            substring(GPS_Struct.str, result, 0, 2);
            GPS_Struct.Day = (uint8_t)atof(GPS_Struct.str);
            substring(GPS_Struct.str, result, 2, 2);
            GPS_Struct.Month = (uint8_t)atof(GPS_Struct.str);
            substring(GPS_Struct.str, result, 4, 2);
            GPS_Struct.Year = (uint8_t)atof(GPS_Struct.str);
            break;
        case(12)://��λģʽ
            GPS_Struct.ModeIndicator = result[0];
            break;
        }
    }
}
//����GPS NMEA GNRMC��Ϣ
void parseGNRMC(GPS_Structure* data)
{
    char result[256];
    for (int i = 1; i < 14; i++) {
        getStringAfterNthComma(data->Buff, i, result, sizeof(result));
    }
}

static UBaseType_t status_value;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance==USART2)
    {
        status_value=taskENTER_CRITICAL_FROM_ISR();
        USART2_RxStruct.Rx_Buff[USART2_RxStruct.Rx_len]=USART2_RxStruct.Receive;
        if(USART2_RxStruct.Rx_Buff[USART2_RxStruct.Rx_len-1]==0x0d && USART2_RxStruct.Rx_Buff[USART2_RxStruct.Rx_len]==0x0a)
        {
            if(USART2_RxStruct.Rx_Buff[3]=='R' && USART2_RxStruct.Rx_Buff[4]=='M')
            {
                for(uint8_t i=0;i<USART2_RxStruct.Rx_len-1;i++)
                {
                    USART2_RxStruct.Buff[i]=USART2_RxStruct.Rx_Buff[i];
                    GPS_Struct.Buff[i]=USART2_RxStruct.Buff[i];
                    USART2_RxStruct.Buff[i]=0;
                }
            }
            USART2_RxStruct.Rx_len=0;
        }
        else
        {
            USART2_RxStruct.Rx_len++;
        }
        HAL_UART_Receive_IT(GPS_UART,&USART2_RxStruct.Receive,1);
        taskEXIT_CRITICAL_FROM_ISR(status_value);
    }
}








