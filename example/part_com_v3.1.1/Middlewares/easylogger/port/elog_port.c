/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */
 
#include <elog.h>
#include "freertos.h"
#include "freertos.h"
#include "cmsis_os2.h"
#include "usart.h"
#include "semphr.h"
#include "com.h"
//#include "com_component.h"
/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    /* add your code here */
    
    return result;
}

/**
 * EasyLogger port deinitialize
 *
 */
void elog_port_deinit(void) {

    /* add your code here */

}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size) {
    
//  static com_task_t task;
//  task.mode|=TX;
//  task.pdate=(uint8_t*)log;
//  task.length=size;
//  extern com_uart_t com;
//  com_communite(&com,&task,0);
//		    com_uart_tx(&com1,0,(uint8_t*)log,size,0);

    	//HAL_UART_Transmit(&huart1,(uint8_t*)log,size,0xffff);
		//com_comunite(&com_test,tx,(uint8_t*)log,size);
	//HAL_Delay(10);
    com_send(com1, log, size,0);

}

/**
 * output lock
 */
void elog_port_output_lock(void) {
    
    /* add your code here */
//    extern osSemaphoreId_t BinarySem_DebugOutPutHandle;//
//		//printf("elog get lock\n");
//	  //osStatus_t state =osSemaphoreAcquire(BinarySem_DebugOutPutHandle,portMAX_DELAY);
//		extern osMutexId_t Mutex_Usart1Handle;
//	  //osStatus_t state =osMutexAcquire(Mutex_Usart1Handle,portMAX_DELAY);
//	
//	
//			extern SemaphoreHandle_t xMutex_Usart1;
////		BaseType_t xStatus;
//	 //xSemaphoreTakeRecursive(xMutex_Usart1, portMAX_DELAY);

}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
    
//    /* add your code here */
//  	extern osSemaphoreId_t BinarySem_DebugOutPutHandle;//由cubemx生成，在freertos.h中创建
//		//printf("elog release lock\n");

//		//osStatus_t state = osSemaphoreRelease(BinarySem_DebugOutPutHandle);  
//	extern osMutexId_t Mutex_Usart1Handle;
//	//osStatus_t state =osMutexRelease(Mutex_Usart1Handle);
//	
//		extern SemaphoreHandle_t xMutex_Usart1;
//	BaseType_t xStatus;
	//xStatus  = xSemaphoreGiveRecursive(xMutex_Usart1);

}

/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void) {
    
    /* add your code here */
    return "";
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    
    /* add your code here */
    return "";

}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {
    
    /* add your code here */
    return "";
}
//以自己的板子为开发平台，1移植freertos（使用cubemx配置选V2）,2 移植multi_Button 驱动一个按键，3展示大家对于信号量，互斥锁，事件等等这些当中的知识
