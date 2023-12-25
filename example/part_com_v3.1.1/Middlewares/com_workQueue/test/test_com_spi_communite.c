#include "minunit.h"
#include "../example/com_port.h"

extern com_t __coms[COM_NUM];

MU_TEST(test_com_spi_tx) {
//创建数据,每个数据长度为8
    uint8_t data[8] = {0x01, 0x02, 0x03, 0x04, 0x05,
                       0x06, 0x07, 0x08};
    uint8_t data2[8] = {0x11, 0x12, 0x13, 0x14, 0x15,
                        0x16, 0x17, 0x18};
    uint8_t data3[8] = {0x21, 0x22, 0x23, 0x24, 0x25,
                        0x26, 0x27, 0x28};
    //发送数据
    mu_assert(com_send(com3, data, sizeof(data),0) == COM_SUCCESS, "com3 write fail");
    mu_assert(com_send(com3, data2, sizeof(data2),0) == COM_SUCCESS, "com3 write fail");
    mu_assert(com_send(com3, data3, sizeof(data3),0) == COM_SUCCESS, "com3 write fail");

    //创建接受任务
    Sleep(1);

}
MU_TEST_SUITE(test_spi_suite) {
    MU_RUN_TEST(test_com_spi_tx);


}