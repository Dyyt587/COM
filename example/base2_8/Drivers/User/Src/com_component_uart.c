#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "com_component.h"
#include "com_component_uart.h"

#include "dqueue.h"
#include "stdbool.h"
#include "usart.h"
#include "kfifo.h"
#include "stdint.h"
#include "elog.h"

#define DEBUG_COM 1

////////////////////////com_uart_part_start////////////////////////////////////////////////////////

void com_put_task(com_t *com);
uint8_t com_uart_tx_cplt(com_t *com);
void com_uart_try_tx(com_t *com);
void com_uart_init(com_t *com, uint32_t buf_size)
{
  com->tx_queue.taskfifo = kfifo_alloc(4096 * 4, 0);
  com->tx_queue.datefifo = kfifo_alloc(buf_size, 0);

  com->task_1.mode = 0;

  COM_TX_UNLOCK(com);
  COM_RX_UNLOCK(com);
}

void com_uart_link(com_t *com, void (*ll_tx)(struct com_t *com))
{
  com->ll_tx = ll_tx;
}

uint8_t com_uart_tx(com_t *com, uint32_t addr, uint8_t *pdata, uint32_t length, uint32_t delaytime)
{
  if (com_queue_put(&com->tx_queue, COM_UART_QUEUE_TX, 0, pdata, length) == 0)
  {
    com->tx_cnt.total_cnt++;
    if (0 == COM_IS_TX_LOCK(com))
    {
      com_uart_try_tx(com);
    }
    else
    {
    }
  }
  else
  {
    //while (1);
		return -1;
  }
	return 0;
}
void com_uart_try_tx(com_t *com)
{
  if (com_check_task(com, &com->tx_queue, &com->task_1) == true)
  {
    COM_TX_LOCK(com);
    com->task_1.pdata = (uint8_t *)kfifo_get_out_pointer(com->tx_queue.datefifo);
    com->ll_tx(com);
  }
  else
  {
    COM_TX_UNLOCK(com);
  }
}

uint8_t com_uart_tx_cplt(com_t *com)
{

  kfifo_get_index(com->tx_queue.datefifo, com->task_1.len);
  com->tx_cnt.cnt++;
  if (kfifo_get_used(com->tx_queue.datefifo) == 0)
  {
    COM_TX_UNLOCK(com);
    // TODO:展示一波错误
    return 255;
  }
  com_uart_try_tx(com);
	    return 0;

}

void com_rx_init(com_t *com, uint32_t size)
{
  com->rx_queue.datefifo = kfifo_alloc(size, 0);
  com->rx_queue.taskfifo = kfifo_alloc(512, 0);
  COM_TX_UNLOCK(com);
  // com->is_rx_busy = 0;
}

// 本函数实现不定长收发函数
uint8_t com_uart_rx_start(com_t *com, uint32_t len)
{
  if (!COM_IS_RX_LOCK(com))
  {
    COM_RX_LOCK(com);

    com->ll_rx(com);
  }
	return 0;
}

// 询问缓冲区并获取数据
uint8_t com_uart_rx(struct com_t *com, uint32_t addr, uint8_t *pdata, uint32_t length, uint32_t delaytime)
{
 // 等待数据
//  uint64_t cnt = com->rx_cnt.total_cnt - 1;
  int8_t error;
  int start_time = com_get_time();
  while (kfifo_get_used(com->rx_queue.datefifo) < length)
  {
    if (delaytime != COM_DELAY_EVERY)
    {
      int time = com_get_time();
      if (time - start_time > delaytime)
      {
        error = -3;
        return error;
      }
    }
  }
  if(kfifo_get(com->rx_queue.datefifo, pdata, length)!=length){
    error = -4;
    return error;
  }
  return 0;
}

uint8_t com_uart_rx_pack_cplt(com_t *com, uint32_t len)
{
  // 将接受的数据放入fifo
	com_queue_put_rx(&com->rx_queue,COM_UART_QUEUE_RX,0,len);
//  com_task_t rx_task = {
////      .magic_word = COMMAGIC_WORD,
//      .pdata = 0,
//      .len = len,
////      .pdata_1 = 0,
//      .mode = COM_UART_QUEUE_RX,
//  };
//  kfifo_put_index(com->rx_queue.datefifo, len);
  com->rx_cnt.total_cnt++;
//  if (kfifo_get_free(com->rx_queue.taskfifo) < sizeof(rx_task))
//    return 255;
//  kfifo_put(com->rx_queue.taskfifo, (uint8_t *)&rx_task, sizeof(rx_task));

  // 查看是否要重新启动任务
	return 0;
}

void com_uart_linkrx(com_t *com, void (*ll_rx)(struct com_t *com))
{
  com->ll_rx = ll_rx;
}
