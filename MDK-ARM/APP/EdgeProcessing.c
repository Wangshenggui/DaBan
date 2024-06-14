#include "EdgeProcessing.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include <math.h>
#include "RTK_usart_it.h"
#include <stdio.h>
#include <string.h>

#include "map_data.h"


//// �������εĶ�����������
//double MapDat[][2] = {
//    {116.436837,39.897967},
//    {116.437129,39.897964},
//    {116.437129,39.897729},
//    {116.436828,39.897743}
//};

LocationJudging_Structure LocationJudging_Struct = {0};



extern double RTK_Longitude;
extern double RTK_Latitude;
extern double RTK_CourseAngle;



//��ʼ��
void EdgeComputing_Init(void)
{
    //Ĭ�϶�����Ȧ��
    LocationJudging_Struct.Flag[0] = 0;
    LocationJudging_Struct.Flag[1] = 0;
    LocationJudging_Struct.Flag[2] = 0;
    LocationJudging_Struct.Flag[3] = 0;
    
    ReadMapDataStrutcure pStrutc = ReadLonAndLat();
    
    uint8_t str[100];
    
    sprintf((char*)str,"�ܹ��� %d ������\r\n",pStrutc.num);
    HAL_UART_Transmit(&huart4, str, strlen((char*)str),1000);
    for(uint8_t i = 0;i<pStrutc.num;i++)
    {
        sprintf((char*)str,"��%d���� : %lf,",i+1,pStrutc.lon[i]);
        HAL_UART_Transmit(&huart4, str, strlen((char*)str),1000);
        
        sprintf((char*)str,"%lf\r\n",pStrutc.lat[i]);
        HAL_UART_Transmit(&huart4, str, strlen((char*)str),1000);
    }
    
    // ��ֵ
    for (int i = 0; i < pStrutc.num; i++)
    {
        LocationJudging_Struct.MapDat[i][0] = pStrutc.lon[i];
        LocationJudging_Struct.MapDat[i][1] = pStrutc.lat[i];
    }
    
    //�����
    LocationJudging_Struct.PointCount = pStrutc.num;
    
    for(uint8_t i = 0;i<LocationJudging_Struct.PointCount;i++)
    {
        sprintf((char*)str,"���ռ���ĵ�%d���� : %lf,",i+1,LocationJudging_Struct.MapDat[i][0]);
        HAL_UART_Transmit(&huart4, str, strlen((char*)str),1000);
        
        sprintf((char*)str,"%lf\r\n",LocationJudging_Struct.MapDat[i][1]);
        HAL_UART_Transmit(&huart4, str, strlen((char*)str),1000);
    }
}


// ���ȷ����ʽ�ľ�γ��ת��Ϊʮ���ƶ���
double dms_to_degrees(double dms) 
{
    double degrees = (int)(dms / 100);  // ȡ����������Ϊ����
    double minutes = dms - degrees * 100; // ȡС��������Ϊ����
    double decimal_degrees = degrees + minutes / 60.0; // ת��Ϊʮ���ƶ���
    return decimal_degrees;
}

// �ж�һ�����Ƿ��ڶ�����ڲ�
bool PointInsidePolygon(uint16_t numVertices, double vertices[][2], double x, double y)
{
    bool inside = false;
    for (int i = 0, j = numVertices - 1; i < numVertices; j = i++)
    {
        if (((vertices[i][1] > y) != (vertices[j][1] > y)) &&
            (x < (vertices[j][0] - vertices[i][0]) * (y - vertices[i][1]) / (vertices[j][1] - vertices[i][1]) + vertices[i][0]))
        {
            inside = !inside;
        }
    }
    return inside;
}

//�ӻ�����ת��Ϊ������
double to_degrees(double radian) 
{
    return radian * 180.0 / M_PI;
}
//�Ӷ�����ת��Ϊ������
#define to_radians(degree)   ((degree) * (M_PI / 180.0))
//��֪λ��1�����롢�Ƕȣ���λ��2
void destination_point(double lat1, double lon1, double distance, double bearing, double* lat2, double* lon2) 
{
    // ������ľ�γ��ת��Ϊ����
    lat1 = to_radians(lat1);
    lon1 = to_radians(lon1);
    bearing = to_radians(bearing);

    // ���ݹ�ʽ����Ŀ�ĵص�γ��
    *lat2 = asin(sin(lat1) * cos(distance / EARTH_RADIUS) +
        cos(lat1) * sin(distance / EARTH_RADIUS) * cos(bearing));

    // ���ݹ�ʽ����Ŀ�ĵصľ���
    *lon2 = lon1 + atan2(sin(bearing) * sin(distance / EARTH_RADIUS) * cos(lat1),
        cos(distance / EARTH_RADIUS) - sin(lat1) * sin(*lat2));

    // �����ת���ض�
    *lat2 = to_degrees(*lat2);
    *lon2 = to_degrees(*lon2);
}



//��Ե����
void EdgeComputing(LocationJudging_Structure *locJudStr)
{
    char str[100];
    UNUSED(str);
    
    double lat2, lon2;
    double templat, templon;
    
//    HAL_SYSTICK_Config(1000000);
//    int s = SysTick->VAL = 0;
    
    //ת��10���ƶ�
    templat = dms_to_degrees(RTK_Latitude);
    templon = dms_to_degrees(RTK_Longitude);
    
    //��1��
    destination_point(templat, templon, 0.14/1000, RTK_CourseAngle, &lat2, &lon2);//���������һ������
    locJudStr->Flag[0] = PointInsidePolygon(locJudStr->PointCount,locJudStr->MapDat, lon2,lat2);
    //��2��
    destination_point(templat, templon, 0.39/1000, RTK_CourseAngle, &lat2, &lon2);
    locJudStr->Flag[1] = PointInsidePolygon(locJudStr->PointCount,locJudStr->MapDat, lon2,lat2);
    //��3��
    destination_point(templat, templon, 0.64/1000, RTK_CourseAngle, &lat2, &lon2);
    locJudStr->Flag[2] = PointInsidePolygon(locJudStr->PointCount,locJudStr->MapDat, lon2,lat2);
    //��4��
    destination_point(templat, templon, 0.89/1000, RTK_CourseAngle, &lat2, &lon2);
    locJudStr->Flag[3] = PointInsidePolygon(locJudStr->PointCount,locJudStr->MapDat, lon2,lat2);
    
    
//    sprintf((char*)str,"%d-%d-%d-%d\n",locJudStr->Flag[0],locJudStr->Flag[1],locJudStr->Flag[2],locJudStr->Flag[3]);
//    HAL_UART_Transmit(&huart4, str, strlen((char*)str),1000);
    
    if(locJudStr->Flag[0]==1 || locJudStr->Flag[1]==1 || locJudStr->Flag[2]==1 || locJudStr->Flag[3]==1)
    {
        HAL_GPIO_TogglePin(led_GPIO_Port,led_Pin);
    }
    else
    {
        HAL_GPIO_TogglePin(RTK_LED_GPIO_Port,RTK_LED_Pin);
    }
    
    
//    uint32_t e = SysTick->VAL;
//    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
//    sprintf(str,"%f>>>%0.11lf,%0.11lf   %d\r\n\r\n", RTK_CourseAngle,lon2, lat2,(1000000-e)/72);
//    HAL_UART_Transmit(RTK_UART, (uint8_t*)str, strlen(str),1000);
    
}













