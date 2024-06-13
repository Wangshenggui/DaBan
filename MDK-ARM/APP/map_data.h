#ifndef _MAP_DATA_H
#define _MAP_DATA_H

#include "main.h"
#include "flash.h"

#include "EdgeProcessing.h"



typedef struct
{
    uint8_t num;
    double lon[POINTCOUNT];
    double lat[POINTCOUNT];
}ReadMapDataStrutcure;
extern ReadMapDataStrutcure ReadMapDataStrutc;


void WriteLonAndLat(uint8_t Sum,uint8_t Num,double lon,double lat);
ReadMapDataStrutcure ReadLonAndLat(void);

void WRMapTest(void);











#endif
