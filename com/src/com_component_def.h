#ifndef COM_COMPONENT_DEF_H
#define COM_COMPONENT_DEF_H

#include "stdint.h"
#include "dqueue.h"
#include "stdbool.h"
#include "kfifo.h"

#define COMMAGIC_WORD 0xFF00FF00
#define COM_DELAY_EVERY 0xffffffff
#define DEBUG_ON(...) printf(#__VA_ARGS__);
// 对于uart的tx和rx 由两个queue组成
// 对于iic, 由一个queue组成
// 对于spi 由两个queue组成,但是taskfifo是同一个，进行特别初始化
#define TX (1)
#define RX (0)

// 其实最好可以封装起来，例如
#define SetBit(VAR, Place) (VAR |= (1 << Place))
#define ClrBit(VAR, Place) (VAR &= ((1 << Place) ^ 255))
#define GET_BIT(x, bit) ((x & (1 << bit)) >> bit) /* 获取第bit位 */

#define COM_TX_LOCK(com) (SetBit(com->lock, TX))
#define COM_TX_UNLOCK(com) (ClrBit(com->lock, TX))

#define COM_RX_LOCK(com) (SetBit(com->lock, RX))
#define COM_RX_UNLOCK(com) (ClrBit(com->lock, TX))

#define COM_IS_TX_LOCK(com) GET_BIT(com->lock, TX)
#define COM_IS_RX_LOCK(com) GET_BIT(com->lock, RX)

typedef struct
{
    kfifo_t *datefifo;
    kfifo_t *taskfifo;
} com_queue_t;

typedef struct
{
    uint32_t total_cnt; // 目前总的任务计数
    uint32_t cnt;       // 当前执行到的任务计数
} com_task_cnt_t;

enum
{
    COM_UART_QUEUE_TX,
    COM_UART_QUEUE_TX_STAGE1 = (COM_UART_QUEUE_TX + 1), // 用于环形缓冲区的调转
    COM_UART_QUEUE_TX_STAGE2 = (COM_UART_QUEUE_TX + 2), // 用于环形缓冲区的调转
    COM_UART_QUEUE_RX,
    COM_UART_QUEUE_RX_ONCE,
    COM_I2C_QUEUE_TX,
    COM_I2C_QUEUE_TX_STAGE1 = (COM_I2C_QUEUE_TX + 1), // 用于环形缓冲区的调转
    COM_I2C_QUEUE_TX_STAGE2 = (COM_I2C_QUEUE_TX + 2), // 用于环形缓冲区的调转
    COM_I2C_QUEUE_RX,
    COM_I2C_QUEUE_RX_STAGE1 = (COM_I2C_QUEUE_TX + 1), // 用于环形缓冲区的调转
    COM_I2C_QUEUE_RX_STAGE2 = (COM_I2C_QUEUE_TX + 2), // 用于环形缓冲区的调转
    COM_SPI_QUEUE_TX,
    COM_SPI_QUEUE_TX_STAGE1 = (COM_SPI_QUEUE_TX + 1), // 用于环形缓冲区的调转
    COM_SPI_QUEUE_TX_STAGE2 = (COM_SPI_QUEUE_TX + 2), // 用于环形缓冲区的调转
    COM_SPI_QUEUE_RX,
};
typedef struct
{
    uint32_t magic_word;
    uint32_t addr;
    uint8_t *pdata;
    uint32_t len;
    uint8_t *pdata_1;
    uint32_t mode;
} com_task_t;

typedef enum
{
    com_type_uart,
    com_type_spi,
    com_type_i2c
} com_device_e;

typedef struct com_t
{
    com_queue_t tx_queue;
    uint8_t lock;

    void (*ll_tx)(struct com_t *com);
    com_task_t task_1;
    com_task_cnt_t tx_cnt;

    uint32_t addr;

    com_queue_t rx_queue;
    com_task_t rx_task;
    void (*ll_rx)(struct com_t *com);
    com_task_cnt_t rx_cnt;

    com_device_e type;

    void (*user_rx_callback)(struct com_t *com);
    void (*user_tx_callback)(struct com_t *com);

    uint8_t (*com_tx)(struct com_t *com, uint32_t addr, uint8_t *pdata, uint32_t length, uint32_t delaytime);

    uint8_t (*com_tx_cplt)(struct com_t *com);
    uint8_t (*com_rx_pack_cplt)(struct com_t *com,uint32_t len);

    //用于直接接收，可能从缓存中获取也可能直接接收
    uint8_t (*com_rx)(struct com_t *com, uint32_t addr, uint8_t *pdata, uint32_t length, uint32_t delaytime);

    uint8_t (*com_rx_start)(struct com_t *com, uint32_t length);//用于启用接收监视

		void *userdata;

} com_t;

typedef struct
{
    uint32_t tx_buf_size;
    uint32_t tx_task_size;
    uint32_t rx_buf_size;
    uint32_t rx_task_size;
} com_init_t;

#endif
