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
#include "sfud.h"
#include "flashdb.h"
#include "fal.h"
#include "semphr.h"
#include "adc.h"
#include "usart.h"
#include "com_component.h"
#include "perf_counter.h"
#include "sht30.h"


#include "elog.h"
#include "tim.h"
#include "tim_pwm.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */
	#include "ins330.h"
	static ins330_t ins330;
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


static uint32_t boot_count = 0;
static time_t boot_time[10] = {0, 1, 2, 3};
/* default KV nodes */
static struct fdb_default_kv_node default_kv_table[] = {
        {"username", "armink", 0}, /* string KV */
        {"password", "123456", 0}, /* string KV */
        {"boot_count", &boot_count, sizeof(boot_count)}, /* int type KV */
        {"boot_time", &boot_time, sizeof(boot_time)},    /* int array type KV */
};
/* KVDB object */
static struct fdb_kvdb kvdb = { 0 };
/* TSDB object */
struct fdb_tsdb tsdb = { 0 };
/* counts for simulated timestamp */
//static int counts = 0;

extern void kvdb_basic_sample(fdb_kvdb_t kvdb);
extern void kvdb_type_string_sample(fdb_kvdb_t kvdb);
extern void kvdb_type_blob_sample(fdb_kvdb_t kvdb);
extern void tsdb_sample(fdb_tsdb_t tsdb);

static fdb_time_t get_time(void)
{
    /* Using the counts instead of timestamp.
     * Please change this function to return RTC time.
     */
    return get_system_ms();
}
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

	ins330_init(&ins330);
	uint8_t data;
  /* Infinite loop */
  for(;;)
  {
		int size = com_rx_inquire_data(&com2);
		for(int i=0;i<size;++i){
			
			com_rx_get(&com2,&data,1,0);
			ins330_handle(&ins330,data);
		}
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

	tim_pwm_init(&pwm0,&htim12,TIM_CHANNEL_1,140000000);
	//tim_pwm_auto_updata_clock(&pwm0);
	HAL_TIM_PWM_Start(&htim12,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim12,TIM_CHANNEL_2);
//		log_d(" %d",pwm0.clock_freq);
//		float freq = tim_pwm_get_freq(&pwm0);
//			//log_d("duty = %f\n",tim_pwm_get_duty(&pwm0));
//		log_d("freq = %f\n",freq);
//			//tim_pwm_auto_updata_clock(&pwm0);

	tim_pwm_set_freq(&pwm0,8000);
	tim_pwm_set_duty(&pwm0,0.5);

  sfud_init();
	fal_init();
	sfud_flash* dev = sfud_get_device(SFUD_W25Q64JV_SPI_DEVICE_INDEX);
	extern void sfud_test(uint32_t addr);
	sfud_test(0);
//	const struct fal_partition *part  = fal_partition_find("fdb_kvdb1");

//if(fal_partition_erase_all(part)!=0)log_e("erase error");
	fdb_err_t result;

#if 0
    { /* KVDB Sample */
        struct fdb_default_kv default_kv;

        default_kv.kvs = default_kv_table;
        default_kv.num = sizeof(default_kv_table) / sizeof(default_kv_table[0]);
        /* set the lock and unlock function if you want */
//        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_LOCK, (void *)lock);
//        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_UNLOCK, (void *)unlock);
        /* Key-Value database initialization
         *
         *       &kvdb: database object
         *       "env": database name
         * "fdb_kvdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
         *              Please change to YOUR partition name.
         * &default_kv: The default KV nodes. It will auto add to KVDB when first initialize successfully.
         *        NULL: The user data if you need, now is empty.
         */
        result = fdb_kvdb_init(&kvdb, "env", "fdb_kvdb1", &default_kv, NULL);

        if (result != FDB_NO_ERR) {
            //return ;
        }

        /* run basic KV samples */
        kvdb_basic_sample(&kvdb);
        /* run string KV samples */
        //kvdb_type_string_sample(&kvdb);
        /* run blob KV samples */
        //kvdb_type_blob_sample(&kvdb);
    }
#endif /* FDB_USING_KVDB */

#if 0
    { /* TSDB Sample */
        /* set the lock and unlock function if you want */
       // fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_LOCK, (void *)lock);
        //fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_UNLOCK, (void *)unlock);
        /* Time series database initialization
         *
         *       &tsdb: database object
         *       "log": database name
         * "fdb_tsdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
         *              Please change to YOUR partition name.
         *    get_time: The get current timestamp function.
         *         128: maximum length of each log
         *        NULL: The user data if you need, now is empty.
         */
				uint64_t counts=0;
        result = fdb_tsdb_init(&tsdb, "log", "fdb_tsdb1", get_time, 128, NULL);
        /* read last saved time for simulated timestamp */
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_GET_LAST_TIME, &counts);

        if (result != FDB_NO_ERR) {
            return ;
        }

        /* run TSDB sample */
        //tsdb_sample(&tsdb);
    }
#endif /* FDB_USING_TSDB */
			char test[]="\nthis is a test\n";
	char test1[]="\nthis is a test1\n";
		log_e("test");
  sht30_init();
  /* Infinite loop */
  for(;;)
  {

//	com_comunite(&com_test,tx,(uint8_t*)test,sizeof(test));
//	com_comunite(&com_test,tx,(uint8_t*)test1,sizeof(test1));
    LED1_Toggle;
//		static int i=2;
//		i=i++;
	//printf("yufewgfuye%d\n",i);
		//log_i("i=%d\n",i);
		//log_d("\tin=%d\n",com.txfifo->in);
		//log_d("\tout=%d\n",com.txfifo->out);
		//log_i("free=%d\n",kfifo_get_free(com1_tx.txfifo));
		//log_d("i=\n");

		//report_tempRH_json();
//		log_d("duty = %f\n",tim_pwm_get_duty(&pwm0));
//		log_d("freq = %f\n",tim_pwm_get_freq(&pwm0));
//		log_d("clock = %d\n",pwm0.clock_freq);

		ins330_show_test(&ins330);

		osDelay(100);	
		
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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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

