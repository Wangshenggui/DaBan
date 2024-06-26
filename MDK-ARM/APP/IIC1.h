#ifndef _IIC1_H
#define _IIC1_H

#include "main.h" 
#include "system.h"
#include "systick.h"


//IO方向设置
#define SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//PB9输入模式
#define SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PB9输出模式
//IO操作函数	 
//#define IIC_SCL    PAout(15) //SCL
//#define IIC_SDA    PBout(7) //SDA	 
//#define READ_SDA   PBin(7)  //输入SDA 
#define IIC_SCL(x)  do{HAL_GPIO_WritePin(IIC1_SCL_GPIO_Port,IIC1_SCL_Pin,x?GPIO_PIN_SET:GPIO_PIN_RESET);}while(0)
#define IIC_SDA(x)  do{HAL_GPIO_WritePin(IIC1_SDA_GPIO_Port,IIC1_SDA_Pin,x?GPIO_PIN_SET:GPIO_PIN_RESET);}while(0)
#define READ_SDA    HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	  

#endif










