#include "stdint.h"
#include <string.h>
#include <stdlib.h>
#include "com_component.h"
#include "com_component_spi.h"

#include "dqueue.h"
#include "stdbool.h"
#include "usart.h"
#include "kfifo.h"
#include "stdint.h"
#include "elog.h"

#define DEBUG_COM 1
////////////////////////com_iic_part_start////////////////////////
void com_spi_try_communite(com_t *com);
// 针对spi全双工同步通讯设计，减少一个fifo的初始化
void com_queue_init_spi(com_queue_t *queue, uint32_t tx_fifo_size, kfifo_t *taskfifo)
{
#if DEBUG_COM
    if (queue == 0)
        while (1)
            ;
#endif
    queue->datefifo = kfifo_alloc(tx_fifo_size, 0);
    queue->taskfifo = taskfifo;
#if DEBUG_COM
    // 判断指针非空
    if (queue->taskfifo == 0 || queue->datefifo == 0)
    {
        while (1)
            ;
    }
#endif
}

void com_spi_tx_rx(com_t *com, uint32_t addr, uint8_t *pdata_tx, uint32_t length, uint32_t delaytime)
{
    uint8_t error = 0;
    // uint64_t cnt = ++com->tx_cnt.total_cnt;
    com->addr = addr;
    error = com_queue_put(&com->tx_queue, COM_SPI_QUEUE_TXRX, com->addr, pdata_tx, length);

    if (error == 0) // 将数据放入队列成功
    {
        com->tx_cnt.total_cnt++;
        if (COM_IS_TX_LOCK(com) == 0)
        {
            com_spi_try_communite(com);
        }
        else
        {
            log_v("com_spi_tx bus busy but put data in queue\n");
        }
        com_tx_wait(com, delaytime);
    }
    else
    {
        log_e("com_spi_tx error\n");
    }
}
uint8_t com_spi_tx(com_t *com, uint32_t addr, uint8_t *pdata_tx, uint32_t length, uint32_t delaytime)
{
    uint8_t error = 0;
    // uint64_t cnt = ++com->tx_cnt.total_cnt;
    com->addr = addr;
    error = com_queue_put(&com->tx_queue, COM_SPI_QUEUE_TX, com->addr, pdata_tx, length);

    if (error == 0) // 将数据放入队列成功
    {
        com->tx_cnt.total_cnt++;
        if (COM_IS_TX_LOCK(com) == 0)
        {
            com_spi_try_communite(com);
        }
        else
        {
            log_v("com_spi_tx bus busy but put data in queue\n");
        }
        com_tx_wait(com, delaytime);
    }
    else
    {
        log_e("com_spi_tx error\n");
    }

    return error;
}

void com_spi_try_communite(com_t *com)
{

    COM_TX_LOCK(com);
    COM_RX_LOCK(com);
    // 检查任务
    if (com_check_task(com, &com->tx_queue, &com->task_1) == false) 
    {
        COM_TX_UNLOCK(com);
        COM_RX_UNLOCK(com);
        return;
    }
    if(com->task_1.mode == COM_SPI_QUEUE_RX || com->task_1.mode == COM_SPI_QUEUE_RX_STAGE1 || com->task_1.mode == COM_SPI_QUEUE_RX_STAGE2){
        com->task_1.pdata = (uint8_t *)kfifo_get_out_pointer(com->rx_queue.datefifo);
        com->ll_rx(com);
    }else if(com->task_1.mode == COM_SPI_QUEUE_TX || com->task_1.mode == COM_SPI_QUEUE_TX_STAGE1 || com->task_1.mode == COM_SPI_QUEUE_TX_STAGE2){
        com->task_1.pdata = (uint8_t *)kfifo_get_out_pointer(com->tx_queue.datefifo);
        com->ll_tx(com);
    }
    else{
        com->task_1.pdata = (uint8_t *)kfifo_get_out_pointer(com->tx_queue.datefifo);
        com->rx_task.pdata = (uint8_t *)kfifo_get_out_pointer(com->rx_queue.datefifo);
        com->ll_txrx(com);
    }

    // 调用用户数据发送或接收函数完成实际发送接收

}

uint8_t com_spi_tx_cplt(com_t *com)
{
    // 调用用户回调
    // TODO:用户任务回调
    // 开启下一个可能的任务
    kfifo_get_index(com->tx_queue.datefifo, com->task_1.len);
    com->tx_cnt.cnt++;

    com_spi_try_communite(com);
    return 0;
}

uint8_t com_spi_start_rx(com_t *com, uint32_t addr, uint32_t length, uint32_t delaytime)
{
	uint8_t error = 0;
    // uint64_t cnt = ++com->tx_cnt.total_cnt;
    com->addr = addr;
    error = com_queue_put_rx(&com->rx_queue, COM_SPI_QUEUE_RX, com->addr,  length);

    if (error == 0) // 将数据放入队列成功
    {
        com->rx_cnt.total_cnt++;
        if (COM_IS_RX_LOCK(com) == 0)
        {
            com_spi_try_communite(com);
        }
        else
        {
            log_v("com_spi_tx bus busy but put data in queue\n");
        }
        com_rx_wait(com, delaytime);
    }
    else
    {
        log_e("com_spi_tx error\n");
    }

    return error;
}

uint8_t com_spi_rx_cplt(com_t *com,uint32_t len)
{
    kfifo_put_index(com->rx_queue.datefifo, com->task_1.len);
		com->rx_cnt.cnt++;
	
	//尝试开始下一个任务
	  com_spi_try_communite(com);
	return 0;

}

