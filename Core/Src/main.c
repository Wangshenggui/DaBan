/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Slave_USART.h"
#include "UpperCom_USART.h"
#include "tim_it.h"
#include "RTK_usart_it.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
RunGuidance_Structure RunGuidance_Struct;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/***********************************************************
****程序全称：数据高低位转换
****程序功能：1001010101011100=0011101010101001
****输 入: 要转换位数 要转换数据
****返 回：转换后的数据
***********************************************************/
int data_to_data(unsigned char num, unsigned int data)
{ //高低位..互换.
    unsigned char i;
    unsigned int m = 0;

    for (i = 0; i < num; i++) {
        m = (m << 1) + (data & 1);
        data >>= 1;
    }
    return(m);
}

/***********************************************************
****程序全称：数据高低位转换
****程序功能：10010101=10101001
****输 入: 要转换数据
****返 回：转换后的数据
***********************************************************/
unsigned char reverse_bit(unsigned char data)
{ //高低位..互换.
    data = (data << 4) | (data >> 4);
    data = ((data & 0x33) << 2) | ((data & 0xCC) >> 2);
    data = ((data & 0x55) << 1) | ((data & 0xAA) >> 1);
    return data;
}
//3，得到指定地址上的一个字节或字
#define MEM_B( x ) ( *( (byte *) (x) ) )
#define MEM_W( x ) ( *( (word *) (x) ) )

//4，求最大值和最小值
#define MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#define MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )

//5，得到一个field在结构体(struct)中的偏移量
#define FPOS( type, field ) \
    /*lint -e545 */ ((dword) & ((type*)0)->field) /*lint +e545 */

//6, 得到一个结构体中field所占用的字节数
#define FSIZ( type, field ) sizeof( ((type *) 0)->field )

//7，按照LSB格式把两个字节转化为一个Word
#define FLIPW( ray ) ( (((word) (ray)[0]) * 256) + (ray)[1] )
//Globle Variate
//8，按照LSB格式把一个Word转化为两个字节
#define FLOPW( ray, val ) \
    (ray)[0] = ((val) / 256); \
    (ray)[1] = ((val) & 0xFF)

//9，得到一个变量的地址（word宽度）
#define B_PTR( var ) ( (byte *) (void *) &(var) )
#define W_PTR( var ) ( (word *) (void *) &(var) )

//10，得到一个字的高位和低位字节
#define WORD_LO(xxx) ((byte) ((word)(xxx) & 255))
#define WORD_HI(xxx) ((byte) ((word)(xxx) >> 8))

//11，返回一个比X大的最接近的8的倍数
#define RND8( x ) ((((x) + 7) / 8 ) * 8 )

//12，将一个字母转换为大写
#define UPCASE( c ) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c) )

//13，判断字符是不是10进值的数字
#define DECCHK( c ) ((c) >= '0' && (c) <= '9')

//14，判断字符是不是16进值的数字
#define HEXCHK( c ) ( ((c) >= '0' && (c) <= '9') ||\
    ((c) >= 'A' && (c) <= 'F') || \
    ((c) >= 'a' && (c) <= 'f'))

//15，防止溢出的一个方法
#define INC_SAT( val ) (val = ((val)+1 > (val)) ? (val)+1 : (val))

//16，返回数组元素的个数
#define ARR_SIZE( a ) ( sizeof( (a) ) / sizeof( (a[0]) ) )

//17，返回一个无符号数n尾的值MOD_BY_POWER_OF_TWO(X, n) = X % (2 ^ n)
#define MOD_BY_POWER_OF_TWO( val, mod_by ) \
    ((dword)(val) & (dword)((mod_by)-1))

/*****************************************************************************
**** Byte2BCD
****
****
*****************************************************************************/
unsigned char Byte2BCD(unsigned char Data)
{
    unsigned char Rslt = 0;

    if (Data >= 99) return 0x99;
    while (Data > 9)
    {
        Data -= 10;
        Rslt += 0x10;
    }
    return(Rslt + Data);
}

/*****************************************************************************
**** BCD2Byte
****
****
*****************************************************************************/
unsigned char BCD2Byte(unsigned char Data)
{
    return(((Data >> 4) & 0x0f) * 10 + (Data & 0x0f));
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_IWDG_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  MX_TIM7_Init();
  MX_UART4_Init();
  MX_USART2_UART_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart1, UART1_RxStruct.Rx_Buff, 200);

    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart3, USART3_RxStruct.Rx_Buff, 200);

    __HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart4, USART4_RxStruct.Rx_Buff, 200);

    HAL_TIM_Base_Start_IT(&htim7);
    HAL_TIM_Base_Start_IT(&htim4);

    //开启定时器输入捕获和更新中断
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3);
    __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
    if (htim->Instance == TIM7)
    {
        TIM7_PeriodElapsedCallback();
    }
    
    if (htim->Instance == TIM4)
        TIM4_PeriodElapsedCallback();
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
        /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
        /* User can add his own implementation to report the file name and line number,
           ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
