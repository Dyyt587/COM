#include "com_component.h"

#include "usart.h"
#include "kfifo.h"
#include "klist.h"
#include "elog.h"
// 本文件实现所有串口设备的相关注册
// com_t com;
// com_t com_test;

 com_t com1;
 com_t com2;
// com_t com1;

void ll_uart_tx(struct com_t *com)
{
#ifdef STM32H7
	SCB_CleanDCache_by_Addr((uint32_t *)com->task_1.pdata, (int32_t)com->task_1.len);
#endif
	//	if(com->task_1.mode == COM_UART_QUEUE_TX_STAGE1)log_d("\t\t\t\tCOM_UART_QUEUE_TX_STAGE1");
	//	if(com->task_1.mode == COM_UART_QUEUE_TX_STAGE2)log_d("\t\t\t\tCOM_UART_QUEUE_TX_STAGE2");
	HAL_UART_Transmit_DMA(com->userdata, com->task_1.pdata, (int32_t)com->task_1.len);
}

void ll_uart_rx(struct com_t *com)
{

	HAL_UARTEx_ReceiveToIdle_DMA(com->userdata, (uint8_t *)com->rx_queue.datefifo->buffer, com->rx_queue.datefifo->size);
}

void com_init_hal_fast(struct com_t *com,void *huart)
{
	com_init_t init;
	init.rx_buf_size = 4096*2;
	init.tx_buf_size = 4096*2;
	init.rx_task_size = 2048;
	init.tx_task_size = 1024*8;
	com_init(com, com_type_uart, &init);
	com_driver_t driver;
	driver.user_rx_driver = ll_uart_rx;
	driver.user_tx_driver = ll_uart_tx;
	driver.user_data = huart;//，目前无用
	com_register_driver(com,&driver);
	com_rx_start(com, com->rx_queue.datefifo->size);
}
void com_user_init(void)
{
	// com_uart_init(&com1, 4096 * 2);
	// com_uart_link(&com1, ll_uart_tx);

	// com_rx_init(&com1, 1024);
	// com_uart_linkrx(&com1, ll_uart_rx);
	// com_uart_rx_start(&com1, com1.tx_queue.datefifo->size);
	com_init_hal_fast(&com1,&huart1);
  	com_init_hal_fast(&com2,&huart4);


}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == com1.userdata)
	{
		com_uart_tx_cplt(&com1);
	}
		if (huart == com2.userdata)
	{
		com_uart_tx_cplt(&com2);
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	// static int puted=0;

	if (huart == com1.userdata)
	{
		uint32_t flag = HAL_UARTEx_GetRxEventType(huart);
		// TODO:对于接收完成事件，判断是否空闲否则拼包
		if (flag == HAL_UART_RXEVENT_IDLE || flag == HAL_UART_RXEVENT_TC)
		{
			// int  a=Size-com1.puted;
			int a = Size - (com1.rx_queue.datefifo->out & (com1.rx_queue.datefifo->size - 1));

			if (a < 0)
				a += com1.rx_queue.datefifo->size;
				// if(a==0)while(1);
#ifdef STM32H7
			unsigned int l;
			l = min(a, (com1.rx_queue.datefifo->out & (com1.rx_queue.datefifo->size - 1)));

			SCB_InvalidateDCache_by_Addr(
				(uint32_t *)(com1.rx_queue.datefifo->buffer + (com1.rx_queue.datefifo->out & (com1.rx_queue.datefifo->size - 1))), l);
			SCB_InvalidateDCache_by_Addr(
				(uint32_t *)com1.rx_queue.datefifo->buffer, a - l);
#endif
			com_uart_rx_pack_cplt(&com1, a);

			// puted=Size;
		}
	}
	
		if (huart == com2.userdata)
	{
		uint32_t flag = HAL_UARTEx_GetRxEventType(huart);
		// TODO:对于接收完成事件，判断是否空闲否则拼包
		if (flag == HAL_UART_RXEVENT_IDLE || flag == HAL_UART_RXEVENT_TC)
		{
			// int  a=Size-com2.puted;
			int a = Size - (com2.rx_queue.datefifo->out & (com2.rx_queue.datefifo->size - 1));

			if (a < 0)
				a += com2.rx_queue.datefifo->size;
				// if(a==0)while(1);
#ifdef STM32H7
			unsigned int l;
			l = min(a, (com2.rx_queue.datefifo->out & (com2.rx_queue.datefifo->size - 1)));

			SCB_InvalidateDCache_by_Addr(
				(uint32_t *)(com2.rx_queue.datefifo->buffer + (com2.rx_queue.datefifo->out & (com2.rx_queue.datefifo->size - 1))), l);
			SCB_InvalidateDCache_by_Addr(
				(uint32_t *)com2.rx_queue.datefifo->buffer, a - l);
#endif
			com_uart_rx_pack_cplt(&com2, a);

			// puted=Size;
		}
	}
	
	
}

// void com_user_init(void)
//{
//	//对com进行必要配置

//}
// CEVENT_EXPORT(EVENT_INIT_STAGE2, userShellInit);
// 串口接收错误中断，下一节会说明为啥要有这个
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (HAL_UART_GetError(huart) & HAL_UART_ERROR_PE)
	{ /*!< Parity error            */
		// 奇偶校验错误
		__HAL_UART_CLEAR_PEFLAG(huart);
	}
	else if (HAL_UART_GetError(huart) & HAL_UART_ERROR_NE)
	{ /*!< Noise error             */
		// 噪声错误
		__HAL_UART_CLEAR_NEFLAG(huart);
	}
	else if (HAL_UART_GetError(huart) & HAL_UART_ERROR_FE)
	{ /*!< Frame error             */
		// 帧格式错误
		__HAL_UART_CLEAR_FEFLAG(huart);
	}
	else if (HAL_UART_GetError(huart) & HAL_UART_ERROR_ORE)
	{ /*!< Overrun error           */
		// 数据太多串口来不及接收错误
		__HAL_UART_CLEAR_OREFLAG(huart);
	}
	// 当这个串口发生了错误，一定要在重新使能接收中断
	if (huart->Instance == USART1)
	{
		while (1)
			;
		// HAL_UART_Receive_IT(&huart1, &rx_temp, 1);
	}
	// 其他串口......
}
