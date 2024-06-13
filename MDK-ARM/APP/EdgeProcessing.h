#ifndef _EDGEPROCESSING_H
#define _EDGEPROCESSING_H

#include "main.h"
#include <stdbool.h>


#define POINTCOUNT 30
typedef struct
{
    uint8_t Flag[4];
    double MapDat[POINTCOUNT][2];
    uint8_t PointCount;//�����
}LocationJudging_Structure;
extern LocationJudging_Structure LocationJudging_Struct;


#define EARTH_RADIUS  6371.0f  // ����ƽ���뾶����λΪ����
#define M_PI    3.1415926f


void EdgeComputing_Init(void);
double dms_to_degrees(double dms);
//�жϵ��Ƿ��ڶ�����ڲ�
bool PointInsidePolygon(uint16_t numVertices, double vertices[][2], double x, double y);
void destination_point(double lat1, double lon1, double distance, double bearing, double* lat2, double* lon2);

//��Ե����
void EdgeComputing(LocationJudging_Structure locJudStr);








#endif
