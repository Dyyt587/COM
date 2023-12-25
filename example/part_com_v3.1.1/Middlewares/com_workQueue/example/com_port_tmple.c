/**
  ******************************************************************************
  * @file           : com_port_tmple.c
  * @author         : 80520
  * @brief          : None
  * @attention      : None
  * @date           : 2023/12/22
  ******************************************************************************
  */


#include "com_port_tmple.h"
static int driver(uint8_t *buf, uint32_t len,uint8_t *buf1, uint32_t len1,  uint32_t mode)
{
    //编写你自己的驱动
    return 0;
}
int com_port_init_tmp(void) {
    //用户初始化
    //1 完成底层硬件初始化(如果有)
    //2 完成com设备模式设置
    com_init_mode(com1,COM_FULL_DUPLEX_MODE);

    //2 将输出输入驱动链接到com设备
    com_register_driver(com1,driver);
    com_register_driver(com1,driver);



//do not change this
    return com_init();
}
