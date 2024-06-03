#include "EdgeProcessing.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include <math.h>
#include "RTK_usart_it.h"
#include <stdio.h>
#include <string.h>


// 定义多边形的顶点坐标数组
double MapDat[][2] = {
    {116.436837,39.897967},
    {116.437129,39.897964},
    {116.437129,39.897729},
    {116.436828,39.897743}
};

LocationJudging_Structure LocationJudging_Struct = {0};



extern double RTK_Longitude;
extern double RTK_Latitude;
extern double RTK_CourseAngle;



//初始化
void EdgeComputing_Init(void)
{
    //默认都不在圈内
    LocationJudging_Struct.Flag[0] = 0;
    LocationJudging_Struct.Flag[1] = 0;
    LocationJudging_Struct.Flag[2] = 0;
    LocationJudging_Struct.Flag[3] = 0;
    
    // 赋值
    for (int i = 0; i < POINTCOUNT; i++)
    {
        LocationJudging_Struct.MapDat[i][0] = MapDat[i][0];
        LocationJudging_Struct.MapDat[i][1] = MapDat[i][1];
    }
    
    //点个数
    LocationJudging_Struct.PointCount = POINTCOUNT;
}


// 将度分秒格式的经纬度转换为十进制度数
double dms_to_degrees(double dms) 
{
    double degrees = (int)(dms / 100);  // 取整数部分作为度数
    double minutes = dms - degrees * 100; // 取小数部分作为分数
    double decimal_degrees = degrees + minutes / 60.0; // 转换为十进制度数
    return decimal_degrees;
}

// 判断一个点是否在多边形内部
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

//从弧度制转换为度数制
double to_degrees(double radian) 
{
    return radian * 180.0 / M_PI;
}
//从度数制转换为弧度制
#define to_radians(degree)   ((degree) * (M_PI / 180.0))
//已知位置1、距离、角度，求位置2
void destination_point(double lat1, double lon1, double distance, double bearing, double* lat2, double* lon2) 
{
    // 将输入的经纬度转换为弧度
    lat1 = to_radians(lat1);
    lon1 = to_radians(lon1);
    bearing = to_radians(bearing);

    // 根据公式计算目的地的纬度
    *lat2 = asin(sin(lat1) * cos(distance / EARTH_RADIUS) +
        cos(lat1) * sin(distance / EARTH_RADIUS) * cos(bearing));

    // 根据公式计算目的地的经度
    *lon2 = lon1 + atan2(sin(bearing) * sin(distance / EARTH_RADIUS) * cos(lat1),
        cos(distance / EARTH_RADIUS) - sin(lat1) * sin(*lat2));

    // 将结果转换回度
    *lat2 = to_degrees(*lat2);
    *lon2 = to_degrees(*lon2);
}



//边缘计算
void EdgeComputing(LocationJudging_Structure locJudStr)
{
    char str[100];
    UNUSED(str);
    
    double lat2, lon2;
    double templat, templon;
    
    HAL_SYSTICK_Config(1000000);
    int s = SysTick->VAL = 0;
    
    //转换10进制度
    templat = dms_to_degrees(RTK_Latitude);
    templon = dms_to_degrees(RTK_Longitude);
    
    //第1个
    destination_point(templat, templon, .014, 45, &lat2, &lon2);//推算出另外一个坐标
    locJudStr.Flag[0] = PointInsidePolygon(LocationJudging_Struct.PointCount,MapDat, lon2,lat2);
    //第2个
    destination_point(templat, templon, .038, 45, &lat2, &lon2);
    locJudStr.Flag[1] = PointInsidePolygon(LocationJudging_Struct.PointCount,MapDat, lon2,lat2);
    //第3个
    destination_point(templat, templon, .064, 45, &lat2, &lon2);
    locJudStr.Flag[2] = PointInsidePolygon(LocationJudging_Struct.PointCount,MapDat, lon2,lat2);
    //第4个
    destination_point(templat, templon, .088, 45, &lat2, &lon2);
    locJudStr.Flag[3] = PointInsidePolygon(LocationJudging_Struct.PointCount,MapDat, lon2,lat2);
    
    
    uint32_t e = SysTick->VAL;
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
    sprintf(str,"%f>>>%0.11lf,%0.11lf   %d\r\n\r\n", RTK_CourseAngle,lon2, lat2,(1000000-e)/72);
    HAL_UART_Transmit(RTK_UART, (uint8_t*)str, strlen(str),1000);
    
}













