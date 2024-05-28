#ifndef _POSIDETER_H
#define _POSIDETER_H

#include "main.h"
#include <stdbool.h>

#define EARTH_RADIUS  6371.0f  // ����ƽ���뾶����λΪ����
#define M_PI    3.1415926f

double dms_to_degrees(double dms);
//�жϵ��Ƿ��ڶ�����ڲ�
bool PointInsidePolygon(uint16_t numVertices, double vertices[][2], double x, double y);
void destination_point(double lat1, double lon1, double distance, double bearing, double* lat2, double* lon2);










#endif
