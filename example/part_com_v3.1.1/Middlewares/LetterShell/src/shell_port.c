/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "shell.h"
#include "stm32h7xx_hal.h"
#include "usart.h"
//#include "log.h"
#include "cmsis_os.h"
#include "stdbool.h"
#include "semphr.h"
#include "stm32h7xx_hal_dma.h"

#include "kfifo.h"
#include "elog.h"
//#include "com_uart_port.h"
#include "string.h"
#include "stdlib.h"
#include "com.h"

Shell shell;
char shellBuffer[512];
//uint8_t rxBuffer[512];
char shelldata=0;
//struct kfifo* shell_rxfifo;
//static SemaphoreHandle_t shellMutex;
//		 extern com_uart_t com;

/**
 * @brief 用户shell写
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际写入的数据长度
 */
short userShellWrite(char *data, unsigned short len)
{

	//HAL_UART_Transmit(&huart1,(uint8_t*)data,len,0xffff);
	com_send(com1,(uint8_t*)data,len,10);
	return len;
}


/**
 * @brief 用户shell读
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际读取到
 */
short userShellRead(char *data, unsigned short len)
{
	
	uint32_t cnt=0;
//	cnt=kfifo_get(com1.rx_queue.datefifo,(uint8_t*)data,len);
  cnt=com_inquire_data(com1,(uint8_t*)data,len);

	if(!cnt){
			osDelay(1);
	}else{
//	char t = *data;
//		log_d("cnt %d",cnt);
//		log_d("data %c",data);
	}
	return cnt;
}
/**
 * @brief 用户shell上锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
int userShellLock(Shell *shell)
{


    return 0;
}

/**
 * @brief 用户shell解锁
 * 
 * @param shell shell
 * 
 * @return int 0       
 */
int userShellUnlock(Shell *shell)
{


  return 0;
}




/**
 * @brief 用户shell初始化
 * 
 */
void userShellInit(void)
{


    shell.write = userShellWrite;
    shell.read = userShellRead;
    shell.lock = userShellLock;
    shell.unlock = userShellUnlock;
    shellInit(&shell, shellBuffer, 512);
		//logRegister(&uartLog, &shell);


//	static 	com_task_t task;//创建一个接收任务
//		task.mode|=RX;
//		task.pdate=com.rx_fifo->buffer;
//		task.length=com.rx_fifo->size;
//		com_communite(&com,&task,0);

		//HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t*)com.rx_fifo->buffer,com.rx_fifo->size);
		//HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t*)shell_rxfifo->buffer,shell_rxfifo->size);

    if (xTaskCreate(shellTask, "shell", 512, &shell, 5, NULL) != pdPASS)
    {
        //logError("shell task creat failed");
    }
}

/* USER CODE BEGIN 1 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//	static int cnt=0;
//	if(huart==&huart1){
//		//extern Shell shell;
//		//extern uint8_t shelldata;	
//		//extern struct kfifo shell_rxfifo;	  
//		uint8_t data=shelldata;//不要删
//		HAL_UART_Receive_IT(&huart1,(uint8_t*)&shelldata,1);//
//		__HAL_UART_CLEAR_IDLEFLAG(&huart1);  				//清除IDLE标志
//__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);       	// 使能 IDLE中断
//		cnt++;
//		kfifo_put(shell_rxfifo,&data,1);
//	  //shellHandler(&shell, data);

//	}

			//HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t*)&rxBuffer,sizeof(rxBuffer));

	
}



