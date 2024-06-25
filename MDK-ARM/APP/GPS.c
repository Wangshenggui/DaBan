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
    /*初始化GPS*/
    if (sys_rst_flag != 99)
    {
        sys_rst_flag = 99;
        sys_rst_n = 9;
    }
    
    if (sys_rst_flag == 99)
        sys_rst_n--;
    if (sys_rst_n > 0 && sys_rst_n < 10)
    {
        //先发一个醒脑
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
//计算检验值
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

// 获取从第n个逗号后开始的字符
//static char getCharAfterNthComma(const char* str, int n) {
//    const char* character = str;

//    // 定位到第n个逗号
//    for (int i = 0; i < n; i++) {
//        character = strchr(character, ',');
//        if (!character) {
//            // 如果逗号不够n个，返回空字符
//            return '\0';
//        }
//        // 移动到下一个字符
//        character++;
//    }

//    // 返回第n个逗号后的字符
//    return *character;
//}
//裁剪字符串
static char* substring(char* dst, char* src, int start, int len)
{
    int length = 100;//最大长度
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
// 获取从第n个逗号后开始的字符串
static void getStringAfterNthComma(const char* string, int n, char* result, size_t max_length) {
    const char* start = string;

    int xor_sum = 0;//校验值
    int i = 0;
    unsigned char len = 0;//计算字符串长度
    char checksum_str[3];

    //计算长度
    while (string[len] != '\0')len++;
    // 计算数据的异或和
    for (i = 1; i < len; i++)
    {
        if (string[i] == '*')
            break;
        xor_sum ^= string[i];
    }
    sprintf(checksum_str, "%02X", xor_sum);
    if (strncmp(checksum_str, (const char*)&string[i + 1], 2) == 0) //通过 
    {
        // 定位到第n个逗号
        for (int i = 0; i < n; i++) {
            start = strchr(start, ',');
            if (!start) {
                // 如果逗号不够n个，将结果设为空字符串
                result[0] = '\0';
                return;
            }

            // 移动到下一个字符
            start++;
        }

        // 计算逗号后字符串的长度
        const char* end = strchr(start, ',');
        size_t length = end ? (size_t)(end - start) : strlen(start);

        // 截取字符串并复制到结果
        if (length < max_length - 1) {
            strncpy(result, start, length);
            // 手动添加 null 结尾
            result[length] = '\0';
        }
        else {
            // 目标长度不足，截断字符串
            strncpy(result, start, max_length - 1);
            result[max_length - 1] = '\0';
        }

        switch (n)
        {
        case(1)://时间
            //解析时间
            substring(GPS_Struct.str, result, 0, 2);
            GPS_Struct.Hour = (uint8_t)atof(GPS_Struct.str) + 8;
            substring(GPS_Struct.str, result, 2, 2);
            GPS_Struct.Min = (uint8_t)atof(GPS_Struct.str);
            substring(GPS_Struct.str, result, 4, 2);
            GPS_Struct.Sec = (uint8_t)atof(GPS_Struct.str);
            break;
        case(2)://定位状态
            GPS_Struct.LocationState = result[0];
            break;
        case(3)://纬度
            //解析纬度
            //度
            substring(GPS_Struct.str, result, 0, 2);
            GPS_Struct.LatitudeDegrees = (uint8_t)atoi(GPS_Struct.str);
            //分
            substring(GPS_Struct.str, result, 2, 2);
            GPS_Struct.LatitudeMinutes = (uint8_t)atoi(GPS_Struct.str);
            /*GPS*/
            if (strlen(result) == 11)
            {
                //秒
                substring(GPS_Struct.str, result, 5, 6);
                GPS_Struct.LatitudeSeconds = (uint8_t)(atof(GPS_Struct.str) * 0.000001 * 60 + 0.5);
            }
            else if (strlen(result) == 9)
            {
                //秒
                substring(GPS_Struct.str, result, 5, 4);
                GPS_Struct.LatitudeSeconds = (uint8_t)(atof(GPS_Struct.str) * 0.0001 * 60 + 0.5);
            }
            /*RTK*/
            else if (strlen(result) == 13)
            {
                //秒
                substring(GPS_Struct.str, result, 5, 8);
                GPS_Struct.LatitudeSeconds = (uint8_t)(atof(GPS_Struct.str) * 0.00000001 * 60 + 0.5);
            }
            break;
        case(4)://N/S
            GPS_Struct.LatitudeDir = result[0];
            break;
        case(5)://经度
            //解析经度
            //度
            substring(GPS_Struct.str, result, 0, 3);
            GPS_Struct.LongitudeDegrees = (uint8_t)atoi(GPS_Struct.str);
            //分
            substring(GPS_Struct.str, result, 3, 2);
            GPS_Struct.LongitudeMinutes = (uint8_t)atoi(GPS_Struct.str);

            /*GPS*/
            if (strlen(result) == 11)
            {
                //秒
                substring(GPS_Struct.str, result, 6, 6);
                GPS_Struct.LongitudeSeconds = (uint8_t)(atof(GPS_Struct.str) * 0.000001 * 60 + 0.5);
            }
            else if (strlen(result) == 9)
            {
                //秒
                substring(GPS_Struct.str, result, 6, 4);
                GPS_Struct.LongitudeSeconds = (uint8_t)(atof(GPS_Struct.str) * 0.0001 * 60 + 0.5);
            }
            /*RTK*/
            else if (strlen(result) == 14)
            {
                //秒
                substring(GPS_Struct.str, result, 6, 8);
                GPS_Struct.LongitudeSeconds = (uint8_t)(atof(GPS_Struct.str) * 0.00000001 * 60 + 0.5);
            }
            break;
        case(6)://E/W
            GPS_Struct.LongitudeDir = result[0];
            break;
        case(7)://地面速度
            GPS_Struct.Speed = (float)atof(result);
            break;
        case(8)://航向角
            GPS_Struct.CourseAngle = (float)atof(result);
            break;
        case(9)://日期
            //解析日期
            substring(GPS_Struct.str, result, 0, 2);
            GPS_Struct.Day = (uint8_t)atof(GPS_Struct.str);
            substring(GPS_Struct.str, result, 2, 2);
            GPS_Struct.Month = (uint8_t)atof(GPS_Struct.str);
            substring(GPS_Struct.str, result, 4, 2);
            GPS_Struct.Year = (uint8_t)atof(GPS_Struct.str);
            break;
        case(12)://定位模式
            GPS_Struct.ModeIndicator = result[0];
            break;
        }
    }
}
//解析GPS NMEA GNRMC信息
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








