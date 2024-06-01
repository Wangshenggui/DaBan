/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Slave_USART.h"
#include "UpperCom_USART.h"
#include "iwdg.h"
#include <string.h>
#include "GPS.h"
#include "system.h"
#include "PosiDeter.h"
#include "RTK_usart_it.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//判断仓是否在圈内
uint8_t LocJudBit[4]={0};
extern double RTK_Longitude;
extern double RTK_Latitude;
extern double RTK_CourseAngle;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//事件组标志
#define IWDG_Event_1 (0x01<<0)
#define IWDG_Event_2 (0x01<<1)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
EventGroupHandle_t IWDG_Event;//创建句柄
/* USER CODE END Variables */
osThreadId SlaveContTaskHandle;
osThreadId EXTIISRTaskHandle;
osThreadId IWDGTaskHandle;
osTimerId UpperRTK_TimerHandle;
osSemaphoreId UpperComBinSemHandle;
osSemaphoreId UARTControlBinSemHandle;
osSemaphoreId PumpOffBinSemHandle;
osSemaphoreId UpperRTK_BinSemHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartSlaveControlTask(void const * argument);
void StartEXTIISRTask(void const * argument);
void StartIWDGTask(void const * argument);
void UpperRTK_TimerCallback(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer, StackType_t** ppxIdleTaskStackBuffer, uint32_t* pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = &xIdleStack[0];
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
    /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
    IWDG_Event = xEventGroupCreate(); //创建事件组
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
      /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of UpperComBinSem */
  osSemaphoreDef(UpperComBinSem);
  UpperComBinSemHandle = osSemaphoreCreate(osSemaphore(UpperComBinSem), 1);

  /* definition and creation of UARTControlBinSem */
  osSemaphoreDef(UARTControlBinSem);
  UARTControlBinSemHandle = osSemaphoreCreate(osSemaphore(UARTControlBinSem), 1);

  /* definition and creation of PumpOffBinSem */
  osSemaphoreDef(PumpOffBinSem);
  PumpOffBinSemHandle = osSemaphoreCreate(osSemaphore(PumpOffBinSem), 1);

  /* definition and creation of UpperRTK_BinSem */
  osSemaphoreDef(UpperRTK_BinSem);
  UpperRTK_BinSemHandle = osSemaphoreCreate(osSemaphore(UpperRTK_BinSem), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
      /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of UpperRTK_Timer */
  osTimerDef(UpperRTK_Timer, UpperRTK_TimerCallback);
  UpperRTK_TimerHandle = osTimerCreate(osTimer(UpperRTK_Timer), osTimerOnce, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
      /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
      /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of SlaveContTask */
  osThreadDef(SlaveContTask, StartSlaveControlTask, osPriorityHigh, 0, 1024);
  SlaveContTaskHandle = osThreadCreate(osThread(SlaveContTask), NULL);

  /* definition and creation of EXTIISRTask */
  osThreadDef(EXTIISRTask, StartEXTIISRTask, osPriorityAboveNormal, 0, 512);
  EXTIISRTaskHandle = osThreadCreate(osThread(EXTIISRTask), NULL);

  /* definition and creation of IWDGTask */
  osThreadDef(IWDGTask, StartIWDGTask, osPriorityLow, 0, 128);
  IWDGTaskHandle = osThreadCreate(osThread(IWDGTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
      /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartSlaveControlTask */
/**
  * @brief  Function implementing the SlaveContTask thread.
  * @param  argument: Not used
  * @retval None
  */
extern//上传标志位
uint8_t UpFlag;
extern 
float RTK_Speed;
/* USER CODE END Header_StartSlaveControlTask */
void StartSlaveControlTask(void const * argument)
{
  /* USER CODE BEGIN StartSlaveControlTask */
    static uint8_t Stop_n = 0;
    
    osThreadSuspend(EXTIISRTaskHandle);
    //初始化GPS
    GPS_Init();
    osThreadResume(EXTIISRTaskHandle);
    
    #include "tim.h"
    //开启定时器输入捕获和更新中断
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3);
    __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
    
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
    hiwdg.Init.Reload = 1875;
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
        Error_Handler();
    }
    /* Infinite loop */
    for (;;)
    {
        //标记事件组
        xEventGroupSetBits(IWDG_Event, IWDG_Event_1);
        
		static uint16_t xxx = 0;
        //水泵开启时
        if (1)
        {
			
            float speed = RTK_Speed*0.5144;//m.s
            
            float L = 0.1;
            
            float value = 31*(speed/L)/10;
            
			if(value > 0x40)
			{
				value = 0x40;
			}
            
            TaskSendSpeed(value);
//            //\
//            1/s   0.2m
//            TaskSendSpeed(15);
//            //\
//            1/s   0.1m
//            TaskSendSpeed(31);
//            
//            //\
//            2/s   0.2m
//            TaskSendSpeed(31);
//            //\
//            2/s   0.1m
//            TaskSendSpeed(62);
			
			HAL_GPIO_TogglePin(led_GPIO_Port,led_Pin);
            
            HAL_GPIO_WritePin(RTK_LED_GPIO_Port,RTK_LED_Pin,GPIO_PIN_SET);
			
			Stop_n = 1;
			xxx=0;
        }
        else//停止信号
        {
			xxx++;
            if (xxx ==10 && Stop_n == 1)
            {
				xxx=0;
				
                HAL_GPIO_WritePin(RTK_LED_GPIO_Port,RTK_LED_Pin,GPIO_PIN_RESET);
                
                Stop_n = 0;
                TaskSendSpeed(0);
                osDelay(10);
                //标记事件组
                xEventGroupSetBits(IWDG_Event, IWDG_Event_1);
            }
			TaskSendRead();
        }
        osDelay(1);
    }
  /* USER CODE END StartSlaveControlTask */
}

/* USER CODE BEGIN Header_StartEXTIISRTask */
/**
* @brief Function implementing the EXTIISRTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartEXTIISRTask */
void StartEXTIISRTask(void const * argument)
{
  /* USER CODE BEGIN StartEXTIISRTask */
      /* Infinite loop */
    for (;;)
    {
        //标记事件组
        xEventGroupSetBits(IWDG_Event, IWDG_Event_2);

        if (osOK == osSemaphoreWait(UARTControlBinSemHandle, 0))
        {
            for (uint8_t i = 0; i < 4; i++)
            {
                uint8_t SpeedTemp;
                Slave_number = i + 1;
                if ((UARTControl_Stru.ButBit >> i) & 0x01)
                {
                    SpeedTemp = UARTControl_Stru.ControlSpeed;
                }
                else
                {
                    SpeedTemp = 0;
                }
                taskENTER_CRITICAL();
                UARTControl_Stru.Dir = 0xc1;
                SendSlaveSpeed(Slave_number, SpeedTemp,UARTControl_Stru.Dir);
				osDelay(5);
				SendSlaveSpeed(Slave_number, SpeedTemp,UARTControl_Stru.Dir);
				osDelay(5);
				SendSlaveSpeed(Slave_number, SpeedTemp,UARTControl_Stru.Dir);
				osDelay(5);
				SendSlaveSpeed(Slave_number, SpeedTemp,UARTControl_Stru.Dir);
				osDelay(5);
				SendSlaveSpeed(Slave_number, SpeedTemp,UARTControl_Stru.Dir);
				osDelay(5);
				SendSlaveSpeed(Slave_number, SpeedTemp,UARTControl_Stru.Dir);
				osDelay(5);
                taskEXIT_CRITICAL();
                //osDelay(5);
            }
        }
        
        if (osOK == osSemaphoreWait(UpperComBinSemHandle, 0))
        {
            //向上位机发送数据
            //解析GPS（RTK已代替）
            parseGNRMC(&GPS_Struct);

            taskENTER_CRITICAL();
            SendSlaveUpperComData();
            taskEXIT_CRITICAL();
        }
        
        if (osOK == osSemaphoreWait(UpperRTK_BinSemHandle, 0))
        {
            //上传RTK配置
            //HAL_GPIO_TogglePin(led_GPIO_Port, led_Pin);
        }

        // 定义多边形的顶点坐标数组
        double MapDat[4][2] = {
            {116.436837,39.897967},
            {116.437129,39.897964},
            {116.437129,39.897729},
            {116.436828,39.897743}
        };
        
        char str[100];
        double lat2, lon2;
        double templat, templon;
        HAL_SYSTICK_Config(1000000);
        taskENTER_CRITICAL();
        int s = SysTick->VAL = 0;
        
        templat = dms_to_degrees(RTK_Latitude);
        templon = dms_to_degrees(RTK_Longitude);
        
        
        destination_point(templat, templon, .14, 0, &lat2, &lon2);
        LocJudBit[0] = PointInsidePolygon(100,MapDat, lon2,lat2);
        
        destination_point(templat, templon, .38, 0, &lat2, &lon2);
        LocJudBit[1] = PointInsidePolygon(100,MapDat, lon2,lat2);
        
        destination_point(templat, templon, .64, 0, &lat2, &lon2);
        LocJudBit[2] = PointInsidePolygon(100,MapDat, lon2,lat2);
        
        destination_point(templat, templon, .88, 0, &lat2, &lon2);
        LocJudBit[3] = PointInsidePolygon(100,MapDat, lon2,lat2);
        
        
        uint32_t e = SysTick->VAL;
        HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
//        sprintf(str,"%f>>>%0.11lf,%0.11lf   %d\r\n\r\n", RTK_CourseAngle,lon2, lat2,(1000000-e)/72);
//        HAL_UART_Transmit_DMA(RTK_UART, str, strlen(str));
        
//        HAL_GPIO_TogglePin(led_GPIO_Port,led_Pin);
        taskEXIT_CRITICAL();
        
        osDelay(1);
    }
  /* USER CODE END StartEXTIISRTask */
}

/* USER CODE BEGIN Header_StartIWDGTask */
/**
* @brief Function implementing the IWDGTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartIWDGTask */
void StartIWDGTask(void const * argument)
{
  /* USER CODE BEGIN StartIWDGTask */
    EventBits_t IWDG_EventBits;
    /* Infinite loop */
    for (;;)
    {
        //1.句柄；2.哪些位；3.读完清零；4.全部有效(与逻辑)；5.等待时间
        IWDG_EventBits = xEventGroupWaitBits(IWDG_Event, IWDG_Event_1 | IWDG_Event_2, pdTRUE, pdTRUE, osWaitForever);

        if ((IWDG_EventBits & (IWDG_Event_1 | IWDG_Event_2)) == (IWDG_Event_1 | IWDG_Event_2))
        {
            /* 如果接收完成并且正确 */
            HAL_IWDG_Refresh(&hiwdg);
        }
    }
  /* USER CODE END StartIWDGTask */
}

/* UpperRTK_TimerCallback function */
void UpperRTK_TimerCallback(void const * argument)
{
  /* USER CODE BEGIN UpperRTK_TimerCallback */
    osSemaphoreRelease(UpperRTK_BinSemHandle);//释放RTK反馈信号量
  /* USER CODE END UpperRTK_TimerCallback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

