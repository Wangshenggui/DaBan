#include "PosiDeter.h"
#include "arm_math.h"


extern double RTK_Longitude;
extern double RTK_Latitude;


/*

168.49C
10636.5702908   2623.06423947
10636.57036765  2623.0639191

10636.5704379   2623.06360254
10636.5705832   2623.06296359

10636.56797268  2623.06653958
10636.56803936  2623.06623197

10636.56811954  2623.06590183
10636.56776201  2623.06583527



106.622872,26.383021
106.622948,26.38311
106.623195,26.382911
106.623087,26.382778
106.622934,26.382895
106.622979,26.38294

*/

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
//double to_radians(double degree) 
//{
//    return degree * M_PI / 180.0;
//}
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













