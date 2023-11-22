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
