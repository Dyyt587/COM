/**
  ******************************************************************************
  * @file           : com.h
  * @author         : 80520
  * @brief          : None
  * @attention      : None
  * @date           : 2023/12/3
  ******************************************************************************
  */

#ifndef UNTITLED_COM_H
#define UNTITLED_COM_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "../src/com_wq.h"
#define COM_DEVICE_CFG(_name,_mode, wq_fifo,_tx_size, wq_fifo_get,_rx_size)do { \
        wq_init(__coms[_name].ws_put, _mode, wq_fifo, 0); \
        wq_init(__coms[_name].ws_get, _mode, wq_fifo_get, 0);                                  \
                kfifo_t *work_fifo = NULL;                                                          \
                KFIFO_INIT(work_fifo, _tx_size, 0);                             \
                __coms[_name].tx_fifo=   work_fifo;                               \
                KFIFO_INIT(work_fifo, _rx_size, 0);                                     \
                __coms[_name].rx_fifo= work_fifo;                                     \
                                                                              \
        }while(0)
#include "com_config.h"

	enum{
        COM_TX,
        COM_TX_CONTINUE,
        COM_PRESERVED,

	    COM_RX,
        COM_RX_CONTINUE,
        COM_PRESERVED_1,

        COM_TXRX,
        COM_TXRX_CONTINUE,
    };
    enum{
        COM_MASTER_SLAVE_MODE,//主从模式
        COM_FULL_DUPLEX_MODE,//全双工模式
	};
    enum{
        hostControlledComMode,//由主机发起通讯的数据，比如iic spi usb(不知道性能是否足够)
        asynchronousComMode,//不受限制的异步通信，例如uart can
    };
typedef int (*ll_driver_t)(uint8_t *txbuf, uint32_t txlen,uint8_t *rxbuf, uint32_t rxlen,uint32_t mode);
typedef struct{
    com_wq_t ws_put;//工作调度器,用户为主机
    com_wq_t ws_get;//工作调度器,接收器为主机
    kfifo_t *rx_fifo;//接收缓存
    kfifo_t *tx_fifo;//发送缓存
    work_task_t tasking_put;
    work_task_t tasking_get;


    ll_driver_t driver;//底层驱动

/*    uint32_t tx_len;//待发送数据长度，如果不为零表示有回转bufyao分两次发送
    uint32_t rx_len;//待接收数据长度，如果不为零表示有回转bufyao分两次发送*/
    uint32_t mode;//通信模式,COM_MASTER_SLAVE_MODE / COM_FULL_DUPLEX_MODE

}com_t;
enum{
    COM_SUCCESS = WQ_SUCCESS,
    COM_PUT_BUSY,
    COM_INVALID_ID,
    COM_FAIL_NULL_POINTER,
};//用于消除空语句警告



enum{
    COM_DEVICE_NAME
            COM_NUM
};
int com_init(void);
int com_register_driver(uint8_t com, ll_driver_t driver);
int com_init_mode(uint8_t com,uint32_t mode);

int com_send(uint8_t com,uint8_t *buf, uint32_t len,uint32_t wait_ms);
int com_send_addr(uint8_t com,uint32_t  addr, uint8_t *buf, uint32_t len,uint32_t wait_ms);

int com_rec(uint8_t com,uint8_t *buf, uint32_t len,uint32_t wait_ms);
int com_rec_addr(uint8_t com,uint32_t  addr,uint8_t *buf, uint32_t len,uint32_t wait_ms);

uint32_t com_inquire_received(uint8_t com);
uint32_t com_read(uint8_t com,uint8_t*pdata,uint32_t length);
int com_cplt(int com,int data_size0);//通常用于任务完成
int com_received(int com,uint32_t data_size0);//通常用于自动接收完成时

//询问缓冲区数据长度
int com_inquire_data(uint8_t com,uint8_t*pdata,uint32_t maxlength);

uint32_t com_get_rxbuf_size(uint8_t com);
uint8_t* com_get_rxbuf_addr(uint8_t com);
uint8_t* com_get_rxbuf_in(uint8_t com);

/*int com_send_cplt(uint8_t com);
int com_send(uint8_t com,uint8_t *buf, uint32_t len);
int com_init(void);
int com_register_driver(uint8_t com, ll_driver_t driver);
uint8_t* com_get_rxbuf_addr(uint8_t com);
uint32_t com_get_rxbuf_size(uint8_t com);

int com_receive(uint8_t com, uint32_t len);
int com_received(uint8_t com, uint32_t len);
uint32_t com_get_rx_data(uint8_t com,uint8_t*pdata,uint32_t length);
uint8_t* com_get_rxbuf_in(uint8_t com);
int com_get_pack(uint8_t com,uint8_t*pdata,uint32_t maxlength);*/
#ifdef __cplusplus
}
#endif
#endif//UNTITLED_COM_H
