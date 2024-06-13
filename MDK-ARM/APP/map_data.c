#include "map_data.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"


ReadMapDataStrutcure ReadMapDataStrutc;




void WRMapTest(void)
{
    WriteLonAndLat(1,1,116.436837,39.897967);
    WriteLonAndLat(2,2,115.436837,38.897967);
    WriteLonAndLat(3,3,15.436837,3.897967);
    
    
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
    
    for(uint8_t i = 0;i<pStrutc.num;i++)
    {
        sprintf((char*)str,"%lf,%lf\r\n",pStrutc.lon[i],pStrutc.lat[i]);
        HAL_UART_Transmit(&huart4, str, strlen((char*)str),1000);
    }
    
}

//д�뾭γ��
void WriteLonAndLat(uint8_t Sum,uint8_t Num,double lon,double lat)
{
    //д���ܹ���ĸ���
    Internal_WriteFlash(0x08030000, (uint16_t *)&Sum, 1);
    
    //д�뾭��
    union
    {
        uint8_t bit[8];
        double value;
    }Longitude;
    Longitude.value = lon;
    Internal_WriteFlash(0x08030004+(Num-1)*16, (uint16_t *)Longitude.bit, 8);
    
    //д��γ��
    union
    {
        uint8_t bit[8];
        double value;
    }Latitude;
    Latitude.value = lat;
    Internal_WriteFlash(0x0803000C+(Num-1)*16, (uint16_t *)Latitude.bit, 8);
}

//��ȡ��γ��
ReadMapDataStrutcure ReadLonAndLat(void)
{
    uint8_t str[100];
    ReadMapDataStrutcure pStrutc;
    
    uint8_t num_temp=*(uint8_t*)0x08030000;
    
    pStrutc.num = num_temp;
    
    union
    {
        uint8_t bit[8];
        double value;
    }ReadData;
    
    for(uint8_t i = 0;i<num_temp;i++)
    {
        Internal_ReadFlash(0x08030004+(i-0)*16, (uint16_t *)ReadData.bit, 8);
        pStrutc.lon[i] = ReadData.value;
        
        Internal_ReadFlash(0x0803000C+(i-0)*16, (uint16_t *)ReadData.bit, 8);
        pStrutc.lat[i] = ReadData.value;
    }
    return pStrutc;
}
    
    
















