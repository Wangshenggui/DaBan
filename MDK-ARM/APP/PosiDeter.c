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
//double to_radians(double degree) 
//{
//    return degree * M_PI / 180.0;
//}
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













