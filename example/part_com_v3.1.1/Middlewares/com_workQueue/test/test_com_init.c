#include "minunit.h"
#include "../example/com_port.h"



extern com_t __coms[COM_NUM];



MU_TEST(test_com_uart_init) {
#define COM_DEVICE_NAME com1
    mu_assert(com_port_init()==COM_SUCCESS, "com init fail");
    //检验初始化的数据是否正确，直接查看com_t结构体的数据
    mu_assert(__coms[COM_DEVICE_NAME].mode == COM_FULL_DUPLEX_MODE, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.type == WS_MODE_ONE_TASKING, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.task_fifo->size == 4096, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.task_fifo->buffer !=0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.task_fifo->in ==0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.task_fifo->out ==0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.is_tasking == 0, "com init fail");

    mu_assert(__coms[COM_DEVICE_NAME].ws_get.type == WS_MODE_ONE_TASKING, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_get.task_fifo->size == 4096, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_get.task_fifo->buffer !=0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_get.task_fifo->in ==0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_get.task_fifo->out ==0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_get.is_tasking == 0, "com init fail");

    mu_assert(__coms[COM_DEVICE_NAME].rx_fifo->size == 1024, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].rx_fifo->buffer !=0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].rx_fifo->in ==0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].rx_fifo->out ==0, "com init fail");

    mu_assert(__coms[COM_DEVICE_NAME].tx_fifo->in ==0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].tx_fifo->out ==0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].tx_fifo->buffer !=0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].tx_fifo->size == 1024, "com init fail");

    mu_assert(__coms[COM_DEVICE_NAME].tasking_put.mode == 0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].tasking_get.mode == 0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].driver != NULL, "com no driver");
/*    mu_assert(__coms[COM_DEVICE_NAME].tx_len == 0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].rx_len == 0, "com init fail");*/
#undef COM_DEVICE_NAME
}


MU_TEST(test_com_iic_init) {
#define COM_DEVICE_NAME com2
    mu_assert(com_port_init()==COM_SUCCESS, "com init fail");
    //检验初始化的数据是否正确，直接查看com_t结构体的数据
    mu_assert(__coms[COM_DEVICE_NAME].mode == COM_MASTER_SLAVE_MODE, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.type == WS_MODE_ONE_TASKING, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.task_fifo->size == 1024, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.task_fifo->buffer !=0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.task_fifo->in ==0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.task_fifo->out ==0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.is_tasking == 0, "com init fail");

    mu_assert(__coms[COM_DEVICE_NAME].ws_get.type == WS_MODE_ONE_TASKING, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_get.task_fifo->size == 1024, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_get.task_fifo->buffer !=0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_get.task_fifo->in ==0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_get.task_fifo->out ==0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_get.is_tasking == 0, "com init fail");

    mu_assert(__coms[COM_DEVICE_NAME].rx_fifo->size == 1024, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].rx_fifo->buffer !=0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].rx_fifo->in ==0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].rx_fifo->out ==0, "com init fail");

    mu_assert(__coms[COM_DEVICE_NAME].tx_fifo->in ==0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].tx_fifo->out ==0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].tx_fifo->buffer !=0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].tx_fifo->size == 1024, "com init fail");

    mu_assert(__coms[COM_DEVICE_NAME].tasking_put.mode == 0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].tasking_get.mode == 0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].driver != NULL, "com no driver");
/*    mu_assert(__coms[COM_DEVICE_NAME].tx_len == 0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].rx_len == 0, "com init fail");*/
#undef COM_DEVICE_NAME
}

MU_TEST(test_com_spi_init) {
#define COM_DEVICE_NAME com3
    mu_assert(com_port_init()==COM_SUCCESS, "com init fail");
    //检验初始化的数据是否正确，直接查看com_t结构体的数据
    mu_assert(__coms[COM_DEVICE_NAME].mode == COM_MASTER_SLAVE_MODE, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.type == WS_MODE_ONE_TASKING, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.task_fifo->size == 1024, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.task_fifo->buffer !=0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.task_fifo->in ==0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.task_fifo->out ==0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_put.is_tasking == 0, "com init fail");

    mu_assert(__coms[COM_DEVICE_NAME].ws_get.type == WS_MODE_ONE_TASKING, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_get.task_fifo->size == 1024, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_get.task_fifo->buffer !=0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_get.task_fifo->in ==0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_get.task_fifo->out ==0, "com init fifo fail");
    mu_assert(__coms[COM_DEVICE_NAME].ws_get.is_tasking == 0, "com init fail");

    mu_assert(__coms[COM_DEVICE_NAME].rx_fifo->size == 1024, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].rx_fifo->buffer !=0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].rx_fifo->in ==0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].rx_fifo->out ==0, "com init fail");

    mu_assert(__coms[COM_DEVICE_NAME].tx_fifo->in ==0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].tx_fifo->out ==0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].tx_fifo->buffer !=0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].tx_fifo->size == 1024, "com init fail");

    mu_assert(__coms[COM_DEVICE_NAME].tasking_put.mode == 0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].tasking_get.mode == 0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].driver != NULL, "com no driver");
/*    mu_assert(__coms[COM_DEVICE_NAME].tx_len == 0, "com init fail");
    mu_assert(__coms[COM_DEVICE_NAME].rx_len == 0, "com init fail");*/
#undef COM_DEVICE_NAME
}

MU_TEST_SUITE(test_init_suite) {
    MU_RUN_TEST(test_com_uart_init);
    MU_RUN_TEST(test_com_iic_init);
    MU_RUN_TEST(test_com_spi_init);


}