#include "stdint.h"
#include <string.h>
#include <stdlib.h>
#include "com_component.h"
#include "com_component_i2c.h"
#include "dqueue.h"
#include "stdbool.h"
#include "usart.h"
#include "kfifo.h"
#include "stdint.h"
#include "elog.h"

#define DEBUG_COM 1
////////////////////////com_iic_part_start////////////////////////

void com_i2c_try_communite(com_t *com);


uint8_t com_i2c_tx(com_t *com, uint32_t addr, uint8_t *pdata, uint32_t length, uint32_t delaytime)
{
    uint8_t error = 0;
    //uint64_t cnt = ++com->tx_cnt.total_cnt;
    com->addr = addr;
    error = com_queue_put(&com->tx_queue, COM_I2C_QUEUE_TX, com->addr, pdata, length);
	
    if (error == 0) // 将数据放入队列成功
    {
			com->tx_cnt.total_cnt++;
			if(COM_IS_TX_LOCK(com) == 0)
        {
            com_i2c_try_communite(com);
        }
        else
        {
            log_v("com_i2c_tx bus busy but put data in queue\n");
        }
        com_tx_wait(com,delaytime);
    }
    else
    {
        log_e("com_i2c_tx error\n");
    }

    return error;
}



void com_i2c_try_communite(com_t *com)
{

		COM_TX_LOCK(com);
		COM_RX_LOCK(com);
    // 检查任务
    if (com_check_task(com, &com->tx_queue,&com->task_1) == false)
		{
			COM_TX_UNLOCK(com);
				COM_RX_UNLOCK(com);
        return;
		}
	
		com->task_1.pdata = (uint8_t*)kfifo_get_out_pointer(com->tx_queue.datefifo);

    // 调用用户数据发送或接收函数完成实际发送接收
		if(com->task_1.mode == COM_I2C_QUEUE_RX || com->task_1.mode == COM_I2C_QUEUE_RX_STAGE1 || com->task_1.mode == COM_I2C_QUEUE_RX_STAGE2){//TODO
			com->ll_rx(com);
		}else{
			com->ll_tx(com);
		}
}

uint8_t com_i2c_tx_cplt(com_t *com)
{
    // 调用用户回调
    // TODO:用户任务回调
    // 开启下一个可能的任务
	  kfifo_get_index(com->tx_queue.datefifo,  com->task_1.len);
	  com->tx_cnt.cnt++;

    com_i2c_try_communite(com);
	return 0;
}

uint8_t com_i2c_start_rx(com_t *com, uint32_t addr, uint8_t *pdata, uint32_t length, uint32_t delaytime)
{
    int8_t error = 0;
    com->addr = addr;
    error = com_queue_put_rx(&com->rx_queue, COM_I2C_QUEUE_RX, com->addr, length); // 放入接收队列
    if (error == 0)                                                                // 将数据放入队列成功
    {
				++com->rx_cnt.total_cnt;
        if (COM_IS_TX_LOCK(com) || COM_IS_RX_LOCK(com))
        {
					log_v("com_i2c_Rx bus busy but put data in queue\n");
					error=-2;
					return error;
        }
        else
        {
					com_i2c_try_communite(com);
            //com_i2c_try_receive(com);
				}
        error = com_rx_wait(com,delaytime);
    }
    else
    {
        log_e("com_i2c_Rx error\n");
								error=-1;

    }

    return error;
}
uint8_t com_i2c_rx_cplt(com_t *com,uint32_t len)
{
    kfifo_put_index(com->rx_queue.datefifo, com->task_1.len);
		com->rx_cnt.cnt++;
	
	//尝试开始下一个任务
	  com_i2c_try_communite(com);
	return 0;

}

void com_i2c_init(com_t *com_i2c)
{
    com_queue_init(&com_i2c->tx_queue, 512, 512);
    com_queue_init_iic(&com_i2c->rx_queue, &com_i2c->tx_queue);
    com_i2c->tx_cnt.cnt = 0;
    com_i2c->rx_cnt.cnt = 0;
    com_i2c->tx_cnt.total_cnt = 0;
    com_i2c->rx_cnt.total_cnt = 0;

		COM_TX_UNLOCK(com_i2c);
		COM_RX_UNLOCK(com_i2c);
    com_i2c->ll_tx = 0;
}

////////////////////////com_iic_part_end////////////////////////////////////////////////////////

