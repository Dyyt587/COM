/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#define LOG_TAG    "freertos"

#include "led.h"
#include "common.h"
#include "semphr.h"
#include "adc.h"
#include "usart.h"
//#include "com_component.h"
#include "perf_counter.h"


#include "elog.h"
#include "tim.h"
#include "spi_w25q64.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
SemaphoreHandle_t xMutex_Usart1;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for testTask */
osThreadId_t testTaskHandle;
uint32_t testTaskBuffer[ 128 ];
osStaticThreadDef_t testTaskControlBlock;
const osThreadAttr_t testTask_attributes = {
  .name = "testTask",
  .cb_mem = &testTaskControlBlock,
  .cb_size = sizeof(testTaskControlBlock),
  .stack_mem = &testTaskBuffer[0],
  .stack_size = sizeof(testTaskBuffer),
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for lowPowerTask */
osThreadId_t lowPowerTaskHandle;
const osThreadAttr_t lowPowerTask_attributes = {
  .name = "lowPowerTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Mutex_Usart1 */
osMutexId_t Mutex_Usart1Handle;
const osMutexAttr_t Mutex_Usart1_attributes = {
  .name = "Mutex_Usart1"
};
/* Definitions for BinarySem_DebugOutPut */
osSemaphoreId_t BinarySem_DebugOutPutHandle;
const osSemaphoreAttr_t BinarySem_DebugOutPut_attributes = {
  .name = "BinarySem_DebugOutPut"
};
/* Definitions for Event_lowPower */
osEventFlagsId_t Event_lowPowerHandle;
const osEventFlagsAttr_t Event_lowPower_attributes = {
  .name = "Event_lowPower"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */




/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTasktest(void *argument);
void StartTask_lowPower(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of Mutex_Usart1 */
  Mutex_Usart1Handle = osMutexNew(&Mutex_Usart1_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
	    xMutex_Usart1 = xSemaphoreCreateRecursiveMutex( );

  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of BinarySem_DebugOutPut */
  BinarySem_DebugOutPutHandle = osSemaphoreNew(1, 1, &BinarySem_DebugOutPut_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of testTask */
  testTaskHandle = osThreadNew(StartTasktest, NULL, &testTask_attributes);

  /* creation of lowPowerTask */
  lowPowerTaskHandle = osThreadNew(StartTask_lowPower, NULL, &lowPowerTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* creation of Event_lowPower */
  Event_lowPowerHandle = osEventFlagsNew(&Event_lowPower_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
__weak void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */

	uint8_t data;
  /* Infinite loop */
  for(;;)
  {

    osDelay(10);
		
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTasktest */


void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc)
{


	//log_d("this is a test of lowerpower callback\r\n");
	osEventFlagsSet(Event_lowPowerHandle, 0x001U); 

	//while(1);


}



/**
* @brief Function implementing the testTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTasktest */
__weak void StartTasktest(void *argument)
{
  /* USER CODE BEGIN StartTasktest */
	extern void userShellInit(void);
	userShellInit();			//

		/* 初始化elog */
	elog_init();
	/* 设置每个级别的日志输出格*/
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
	//输出日志级别信息和日志TAG
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG);
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG);
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG);
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_LVL | ELOG_FMT_TAG);
	//除了时间、进程信息线程信息之外，其余全部输出
	//elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_TIME | ELOG_FMT_P_INFO | ELOG_FMT_T_INFO));
	
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL);
	elog_set_text_color_enabled(true);
	/* 启动elog */
	elog_start();
	
	

	extern void var_init(void);
	var_init();

	HAL_TIM_PWM_Start(&htim12,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim12,TIM_CHANNEL_2);


  //SPI_W25Qxx_Init();
  /* Infinite loop */
  for(;;)
  {


    LED1_Toggle;


		osDelay(500);
		
//		    static char str[]="tast1234567890 12345\r\n";
//    com_tx(&com,(uint8_t*)str,sizeof(str));
//		 //com_communite(&com,&task,0);

  }
  /* USER CODE END StartTasktest */
}

/* USER CODE BEGIN Header_StartTask_lowPower */
uint32_t flags ;

void lowPowerHandle(void)
{
		if(flags==1){
			//log_d("Low power !! please charge\r\n");
		}
}

uint32_t ADC_ConvertedValue;

/**
* @brief Function implementing the lowPowerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_lowPower */
__weak void StartTask_lowPower(void *argument)
{
  /* USER CODE BEGIN StartTask_lowPower */
	static int i=0;
  /* Infinite loop */
  for(;;)
  {
		//log_d("test1234567890");		
		//log_d("%d\n\n",i++);
		    osDelay(60);

		//log_d("test1234567890");

  }
  /* USER CODE END StartTask_lowPower */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	ADC_ConvertedValue = HAL_ADC_GetValue(hadc);

	//log_d("adc value %d\r\n",ADC_ConvertedValue);

  HAL_ADC_Start_IT(hadc); //�????启ADC中断转换


}

/* USER CODE END Application */

