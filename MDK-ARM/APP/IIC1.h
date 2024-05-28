#ifndef _IIC1_H
#define _IIC1_H

#include "main.h" 
#include "system.h"
#include "systick.h"


//IO��������
#define SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//PB9����ģʽ
#define SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PB9���ģʽ
//IO��������	 
//#define IIC_SCL    PAout(15) //SCL
//#define IIC_SDA    PBout(7) //SDA	 
//#define READ_SDA   PBin(7)  //����SDA 
#define IIC_SCL(x)  do{HAL_GPIO_WritePin(IIC1_SCL_GPIO_Port,IIC1_SCL_Pin,x?GPIO_PIN_SET:GPIO_PIN_RESET);}while(0)
#define IIC_SDA(x)  do{HAL_GPIO_WritePin(IIC1_SDA_GPIO_Port,IIC1_SDA_Pin,x?GPIO_PIN_SET:GPIO_PIN_RESET);}while(0)
#define READ_SDA    HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	  

#endif










