#include "minunit.h"
#include "../example/com_port.h"

extern com_t __coms[COM_NUM];

MU_TEST(test_com_iic_tx) {
//创建数据,每个数据长度为8
    uint8_t data[8] = {0x01, 0x02, 0x03, 0x04, 0x05,
                       0x06, 0x07, 0x08};
    uint8_t data2[8] = {0x11, 0x12, 0x13, 0x14, 0x15,
                        0x16, 0x17, 0x18};
    uint8_t data3[8] = {0x21, 0x22, 0x23, 0x24, 0x25,
                        0x26, 0x27, 0x28};
    //发送数据
    mu_assert(com_send(com2, data, sizeof(data),0) == COM_SUCCESS, "com2 write fail");
    mu_assert(com_send(com2, data2, sizeof(data2),0) == COM_SUCCESS, "com2 write fail");
    mu_assert(com_send(com2, data3, sizeof(data3),0) == COM_SUCCESS, "com2 write fail");

    //创建接受任务
    Sleep(1);

}
MU_TEST(test_com_iic_rx) {
//模拟接收数据
//先直接操作buffer放入验证数据
    for (int i = 0; i < __coms[com2].rx_fifo->size; i++) {
        __coms[com2].rx_fifo->buffer[i] = (uint8_t) i;
    }
//验证放入数据
    for (int i = 0; i < __coms[com2].rx_fifo->size; i++) {
        mu_assert(__coms[com2].rx_fifo->buffer[i] == (uint8_t) i, "com2 rx buffer init fail");
    }
    //验证接收数据
    uint8_t buf[1024];
    com_rec_addr(com2,  0xff00ff,buf,__coms[com2].rx_fifo->size,0);
    Sleep(1);

    //验证数据的fifo in out 指针
    mu_assert(__coms[com2].rx_fifo->out == 0, "com2 rx buffer in out pointer error");
    mu_assert(__coms[com2].rx_fifo->in == __coms[com2].rx_fifo->size, "com2 rx buffer in out pointer error");
    //取出数据
    uint8_t data[8] = {0};
    for (int i = 0; i < 8; i++) {
        mu_assert(com_read(com2, data, sizeof(data)) == sizeof(data), "com2 read fail");
        for (int j = 0; j < sizeof(data); j++) {
            mu_assert(data[j] == (uint8_t) (i * sizeof(data) + j), "com2 read data error");
        }
    }

}
MU_TEST(test_com_iic_txrx) {
//同时创建发送和接收任务，验证是否会冲突
//重新复位fifo
    kfifo_reset(__coms[com2].rx_fifo);
    kfifo_reset(__coms[com2].tx_fifo);
//创建数据,每个数据长度为8
    uint8_t data[8] = {0x01, 0x02, 0x03, 0x04, 0x05,
                       0x06, 0x07, 0x08};
    uint8_t data2[8] = {0x11, 0x12, 0x13, 0x14, 0x15,
                        0x16, 0x17, 0x18};
    uint8_t data3[8] = {0x21, 0x22, 0x23, 0x24, 0x25,
                        0x26, 0x27, 0x28};
    //发送数据
    mu_assert(com_send(com2, data, sizeof(data),0) == COM_SUCCESS, "com2 write fail");
    mu_assert(com_send(com2, data2, sizeof(data2),0) == COM_SUCCESS, "com2 write fail");
    mu_assert(com_send(com2, data3, sizeof(data3),0) == COM_SUCCESS, "com2 write fail");

    //创建接受任务
    uint8_t buf[1024];
    com_rec_addr(com2,  0xff00ff,buf,__coms[com2].rx_fifo->size,0);
    Sleep(10);
    //验证数据的fifo in out 指针
    //打印out指针
    printf("com2 rx buffer out pointer:%d\n",__coms[com2].rx_fifo->out);
    mu_assert(__coms[com2].rx_fifo->out == 0, "com2 rx buffer in out pointer error");
    mu_assert(__coms[com2].rx_fifo->in == __coms[com2].rx_fifo->size, "com2 rx buffer in out pointer error");
    //取出数据
    uint8_t data4[8] = {0};
    for (int i = 0; i < 8; i++) {
        mu_assert(com_read(com2, data4, sizeof(data4)) == sizeof(data4), "com2 read fail");
        for (int j = 0; j < sizeof(data4); j++) {
            mu_assert(data4[j] == (uint8_t) (i * sizeof(data4) + j), "com2 read data error");
        }
    }

}
MU_TEST_SUITE(test_iic_suite) {
    MU_RUN_TEST(test_com_iic_tx);
    MU_RUN_TEST(test_com_iic_rx);
    MU_RUN_TEST(test_com_iic_txrx);


}