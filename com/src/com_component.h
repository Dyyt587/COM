#ifndef COM_COMPONENT_H
#define COM_COMPONENT_H

#include "com_component_def.h"

#include "com_component_i2c.h"
#include "com_component_spi.h"
#include "com_component_uart.h"

extern com_t com1;
extern com_t com2;

typedef struct
{
    void (*user_rx_driver)(struct com_t *com);
    void (*user_tx_driver)(struct com_t *com);
    void* user_data;
} com_driver_t;
// 通用函数们

int com_get_time(void);

int8_t com_tx_wait(com_t *com, uint32_t delaytime);
int8_t com_tx_wait(com_t *com, uint32_t delaytime);
void com_queue_init(com_queue_t *queue, uint32_t task_fifo_size, uint32_t tx_fifo_size);
void com_queue_init_spi(com_queue_t *queue, uint32_t tx_fifo_size, kfifo_t *taskfifo);
void com_queue_init_iic(com_queue_t *rx_queue, const com_queue_t *tx_queue);
int8_t com_queue_put(com_queue_t *queue, uint32_t mode, uint32_t addr, uint8_t *pdata, uint32_t length);
int8_t com_queue_put_rx(com_queue_t *queue, uint32_t mode, uint32_t addr, uint32_t length);
bool com_check_task(com_t *com, com_queue_t *queue, com_task_t *task);
int8_t com_rx_wait(com_t *com, uint32_t delaytime);

/////////////////////com驱动顶层函数///////////////////////////
void com_tx(com_t *com, uint8_t *pdata, uint32_t addr, uint32_t length, uint32_t delaytime);
uint32_t com_tx_inquire_data(com_t *com); // 获取缓冲区数据总量

uint32_t com_tx_inquire_task(com_t *com); // 获取缓冲区数据包个数

void com_rx_start(com_t *com, uint32_t length);                                                   // length对于uart设备无效，默认dma会使用所有缓冲区
uint32_t com_rx_inquire_data(com_t *com);                                                         // 获取缓冲区数据区数据
uint32_t com_rx_inquire_pack(com_t *com);                                                         // 获取缓冲区分包个数
uint32_t com_rx_get(com_t *com, uint8_t *pdata, uint32_t length, uint32_t delaytime);             // 从数据缓冲区获取数据
uint32_t com_rx_pack_get(com_t *com, uint8_t *pdata, uint32_t max_buflength, uint32_t delaytime); // 从数据缓冲区获取一个数据包的数据

void com_init(com_t *com, com_device_e type, com_init_t* init);

void com_register_driver(com_t *com, com_driver_t* driver);
void com_register_tx_callback(com_t *com, void (*user_rx_callback)(struct com_t *com));
void com_register_rx_callback(com_t *com, void (*user_rx_callback)(struct com_t *com));


uint8_t com_rx_pack_cplt(com_t *com, uint32_t len);
uint8_t com_tx_cplt(com_t *com);

////////////////////////////////////////////////////////////

void com_user_init(void);
#endif
