#ifndef _GPS_H
#define _GPS_H

#include "main.h"
#include "usart.h"


#define GPS_UART           &huart2


#define Rx_LENG 200

// 定义结构体存储解析后的数据
typedef struct
{
    char Buff[200];
    
    char str[100];
    uint8_t Hour;
    uint8_t Min;
    uint8_t Sec;

    uint8_t Year;
    uint8_t Month;
    uint8_t Day;

    uint16_t LatitudeDegrees;
    uint8_t LatitudeMinutes;
    uint8_t LatitudeSeconds;

    uint16_t LongitudeDegrees;
    uint8_t LongitudeMinutes;
    uint8_t LongitudeSeconds;

    float Speed;
    float CourseAngle;

    uint8_t ModeIndicator;
    uint8_t LocationState;

    uint8_t LatitudeDir;
    uint8_t LongitudeDir;
}GPS_Structure;
extern GPS_Structure GPS_Struct;


typedef struct
{
    uint8_t Rx_Buff[200];
    uint8_t Rx_len;
    uint8_t Buff[200];
    uint8_t Receive;
}USART2_RxStructure;
extern USART2_RxStructure USART2_RxStruct;



void GPS_Init(void);
void parseGNRMC(GPS_Structure* data);






#endif
