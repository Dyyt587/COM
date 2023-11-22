#include "stdint.h"
#include <string.h>
#include <stdlib.h>
#include "com_component.h"
#include "dqueue.h"
#include "stdbool.h"
#include "usart.h"
#include "kfifo.h"
#include "stdint.h"
#include "elog.h"
#include "perf_counter.h"

#define DEBUG_COM 1
////////////////////////com_iic_part_start////////////////////////

int com_get_time(void)
{
    // static int time = 0;
    return get_system_us();
}
int8_t com_tx_wait(com_t *com, uint32_t delaytime)
{
    uint64_t cnt = com->tx_cnt.total_cnt - 1;
    int8_t error;
    int start_time = com_get_time();
    while (cnt > com->tx_cnt.cnt)
    {
        if (delaytime != COM_DELAY_EVERY)
        {
            int time = com_get_time();
            if (time - start_time > delaytime)
            {
                error = -3;
                break;
            }
        }
    }
    return error;
}
int8_t com_rx_wait(com_t *com, uint32_t delaytime)
{
    uint64_t cnt = com->rx_cnt.total_cnt - 1;
    int8_t error;
    int start_time = com_get_time();
    while (cnt > com->rx_cnt.cnt)
    {
        if (delaytime != COM_DELAY_EVERY)
        {
            int time = com_get_time();
            if (time - start_time > delaytime)
            {
                error = -3;
                break;
            }
        }
    }
    return error;
}
void com_queue_init(com_queue_t *queue, uint32_t task_fifo_size, uint32_t tx_fifo_size)
{
#if DEBUG_COM
    if (queue == 0)
        while (1)
            ;
#endif
    queue->taskfifo = kfifo_alloc(task_fifo_size, 0);
    queue->datefifo = kfifo_alloc(tx_fifo_size, 0);

#if DEBUG_COM
    // 判断指针非空
    if (queue->taskfifo == 0 || queue->datefifo == 0)
    {
        while (1)
            ;
    }
#endif
}
// 针对spi全双工同步通讯设计，减少一个queue的初始化
void com_queue_init_iic(com_queue_t *rx_queue, const com_queue_t *tx_queue)
{
#if DEBUG_COM
    if (rx_queue == 0 || tx_queue == 0)
        while (1)
            ;
#endif
    rx_queue->taskfifo = tx_queue->taskfifo;
    rx_queue->datefifo = tx_queue->datefifo;
#if DEBUG_COM
    // 判断指针非空
    if (rx_queue->taskfifo == 0 || rx_queue->datefifo == 0)
    {
        while (1)
            ;
    }
#endif
}

void com_queue_free(com_queue_t *queue)
{
    //    kfifo_free(queue->taskfifo);
    //    kfifo_free(queue->datefifo);
}

/**
 * @brief 将数据放入队列中
 *
 * @param queue 队列指针
 * @param mode 模式
 * @param addr 器件地址，选择性使用
 * @param pdata 数据指针
 * @param length 数据长度
 * @return int8_t 返回值，0表示成功，-1表示数据fifo无空间，-2表示任务fifo无空间
 */
int8_t com_queue_put(com_queue_t *queue, uint32_t mode, uint32_t addr, uint8_t *pdata, uint32_t length)
{
    com_task_t task = {
    //    .magic_word = COMMAGIC_WORD,
        .pdata = pdata,
        .len = length,
//        .pdata_1 = pdata,
        .mode = mode,
        .addr = addr};

    if (kfifo_get_free(queue->datefifo) < length)
    {
        log_e("no memory of data fifo\n");

        return -1;
        // while(1);
    }
    if (kfifo_get_free(queue->taskfifo) < sizeof(task))
    {
        log_e("no memory task fifo\n");

        return -2;
        // while(1);
    }

    // 使用判断是否能放下，不能就分两个任内务
    uint32_t l = min(length, queue->datefifo->size - (queue->datefifo->in & (queue->datefifo->size - 1)));
    if (l < length)
    {
        // 分两次放入

        // 将一个任务拆分成两个任务，TODO:注意txcnt的更新

        kfifo_put(queue->datefifo, pdata, length);

        task.len = l;
        task.mode += 1;
        kfifo_put(queue->taskfifo, (uint8_t *)&task, sizeof(task));
        task.len = length - l;
        task.mode += 1;
        kfifo_put(queue->taskfifo, (uint8_t *)&task, sizeof(task));
        // while(1);
    }
    else
    {
        kfifo_put(queue->datefifo, pdata, length);
        kfifo_put(queue->taskfifo, (uint8_t *)&task, sizeof(task));
    }

    return 0;
}
/**
 * @brief 将数据放入队列中
 *
 * @param queue 队列指针
 * @param mode 模式
 * @param addr 器件地址，选择性使用
 * @param pdata 数据指针
 * @param length 数据长度
 * @return int8_t 返回值，0表示成功，-1表示数据fifo无空间，-2表示任务fifo无空间
 */
int8_t com_queue_put_rx(com_queue_t *queue, uint32_t mode, uint32_t addr, uint32_t length)
{
    com_task_t task = {
  //      .magic_word = COMMAGIC_WORD,
        .pdata = queue->datefifo->buffer + (queue->datefifo->out & (queue->datefifo->size - 1)),
        .len = length,
//        .pdata_1 = queue->datefifo->buffer + (queue->datefifo->out & (queue->datefifo->size - 1)),
        .mode = mode,
        .addr = addr};

    if (kfifo_get_free(queue->datefifo) < length)
    {
        log_e("no memory of data fifo\n");

        return -1;
        // while(1);
    }
    if (kfifo_get_free(queue->taskfifo) < sizeof(task))
    {
        log_e("no memory task fifo\n");

        return -2;
        // while(1);
    }
    kfifo_put(queue->taskfifo, (uint8_t *)&task, sizeof(task));
    return 0;
}

int8_t com_queue_get(com_queue_t *queue, uint8_t *pdata, uint32_t *length)
{
    return 0;
}

bool com_check_task_right(com_t *com, com_queue_t *queue, com_task_t *task)
{
    // if ((task->magic_word == COMMAGIC_WORD) )//&& (task->pdata == task->pdata_1))
    // {
    //     //        if (kfifo_get_used(queue->datefifo) < com->task.len)
    //     //        {
    //     //            // 数据有问题，退出
    //     //            // TODO:待完善解决方案
    //     //          log_e("iic date error!!!!!!!!!!!\n");
    //     //					COM_TX_UNLOCK(com);
    //     //					COM_RX_UNLOCK(com);
    //     //
    //     //          return false;
    //     //        }
    //     return true;
    // }
    // else
    // {
    //     log_e("task date error!!!!!!!!!!!\n");

    //     return false;
    // }
    return true;
}

bool com_check_task(com_t *com, com_queue_t *queue, com_task_t *task)
{
    // 检查任务，没有就返回了
    // if (com_check_task(com, queue) == false)
    //     return false;
    if (kfifo_get_used(queue->taskfifo) <= sizeof(com_task_t))
    {
        return false;
    }
    // 有任务，获取任务
    kfifo_get_try(queue->taskfifo, (uint8_t *)task, sizeof(com_task_t));

    if (com_check_task_right(com, queue, task) == false)
    {
        // 数据检测
        // TODO:如果失败了应该如何检测纠正
        return false;
    }
    kfifo_get_index(queue->taskfifo, sizeof(com_task_t));

    return true;
}

void com_tx(com_t *com, uint8_t *pdata, uint32_t addr, uint32_t length, uint32_t delaytime)
{
    com->com_tx(com, addr, pdata, length, delaytime);
}
uint32_t com_tx_inquire_data(com_t *com) // 获取缓冲区数据总量
{
    uint32_t length = 0;
    length = kfifo_get_used(com->tx_queue.datefifo);
    return length;
}
uint32_t com_tx_inquire_task(com_t *com) // 获取缓冲区数据包个数
{
    uint32_t length = 0;
    length = kfifo_get_used(com->tx_queue.taskfifo) / sizeof(com_task_t);
    return length;
}

void com_rx_start(com_t *com, uint32_t length) // length对于uart设备无效，默认dma会使用所有缓冲区
{
    com->com_rx_start(com, length);
}
uint32_t com_rx_inquire_data(com_t *com) // 获取缓冲区数据区数据
{
    uint32_t length = 0;
    length = kfifo_get_used(com->rx_queue.datefifo);
    return length;
}
uint32_t com_rx_inquire_pack(com_t *com) // 获取缓冲区分包个数
{
    uint32_t length = 0;
    length = kfifo_get_used(com->rx_queue.taskfifo) / sizeof(com_task_t);
    return length;
}
uint32_t com_rx_get(com_t *com, uint8_t *pdata, uint32_t length, uint32_t delaytime) // 从数据缓冲区获取数据
{
	return kfifo_get(com->rx_queue.datefifo,pdata,length);
}
uint32_t com_rx_pack_get(com_t *com, uint8_t *pdata, uint32_t max_buflength, uint32_t delaytime) // 从数据缓冲区获取一个数据包的数据
{
	return 0;
}

void com_init(com_t *com, com_device_e type, com_init_t* init)
{
    memset(com, 0, sizeof(com_init_t));
    com->type = type;

    com->tx_queue.taskfifo = kfifo_alloc(init->tx_task_size, 0);
    com->tx_queue.datefifo = kfifo_alloc(init->tx_buf_size, 0);
    com->rx_queue.taskfifo = kfifo_alloc(init->rx_task_size, 0);
    com->rx_queue.datefifo = kfifo_alloc(init->rx_buf_size, 0);

    if (com->tx_queue.taskfifo == NULL)
    {
        log_e("tx task fifo init error\n");
    }  
    if (com->tx_queue.datefifo == NULL)
    {
        log_e("tx data fifo init error\n");
    }
    if (com->rx_queue.taskfifo == NULL)
    {
        log_e("rx task fifo init error\n");
    }
    if (com->rx_queue.datefifo == NULL)
    {
        log_e("rx data fifo init error\n");
    }
    COM_TX_UNLOCK(com);
    COM_RX_UNLOCK(com);
    switch(type)
    {
        case com_type_uart:
        com->com_tx = com_uart_tx;
        com->com_rx_start = com_uart_rx_start; 
        com->com_tx_cplt = com_uart_tx_cplt;
        com->com_rx_pack_cplt = com_uart_rx_pack_cplt;
            break;
        case com_type_spi:
        //TODO:
            break;
        case com_type_i2c:
        com->com_tx = com_i2c_tx;
        //com->com_rx_start = com_i2c_start_rx;
				com->com_rx= com_i2c_start_rx;
        com->com_tx_cplt = com_i2c_tx_cplt;
        com->com_rx_pack_cplt = com_i2c_rx_cplt;
            break;

        default:
            break;
    }
}
void com_register_driver(com_t *com, com_driver_t* driver)
{
    com->ll_rx = driver->user_rx_driver;
    com->ll_tx = driver->user_tx_driver;
    com->userdata = driver->user_data;
}
//void com_register_tx_callback(com_t *com, void (*user_tx_callback)(struct com_t *com))
//{
//    com->user_tx_callback = user_tx_callback;
//}
//void com_register_rx_callback(com_t *com, void (*user_rx_callback)(struct com_t *com))
//{
//    com->user_rx_callback = user_rx_callback;
//}

uint8_t com_rx_pack_cplt(com_t *com, uint32_t len)
{
    return com->com_rx_pack_cplt(com, len);
}
uint8_t com_tx_cplt(com_t *com)
{
    return com->com_tx_cplt(com);
}
