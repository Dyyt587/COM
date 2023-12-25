/**
  ******************************************************************************
  * @file           : com_port.c
  * @author         : 80520
  * @brief          : None
  * @attention      : None
  * @date           : 2023/12/3
  ******************************************************************************
  */
#include "com_port.h"
#include <stdio.h>
#include <stdio.h>
#include <Windows.h>
#include <process.h>



/* 线程函数声明 */
void Thread1(void*);
void Thread2(void*);
void Thread3(void*);

/* 线程句柄 */
HANDLE h1,h2,h3;
/* 线程共享内存 */
volatile int is_start_dma_uart = 0;
volatile int is_end_dma_uart = 0;
volatile int is_start_dma_iic = 0;
volatile int is_end_dma_iic = 0;
volatile int is_start_dma_spi = 0;
volatile int is_end_dma_spi = 0;

volatile int data_uart = 0;
volatile int data_iic = 0;
volatile int data_spi = 0;

static int test_driver(uint8_t* buf,uint32_t len,uint8_t* buf1,uint32_t len1,uint32_t mode){

    if(mode == COM_TX || mode == COM_TX_CONTINUE){
        printf("com port send data:\n");
        for(int i = 0; i < len; i++){
            printf("%02x ",buf[i]);
            if((i+1)%8 == 0){
                printf("\n");
            }
        }
        printf("\n");
        is_start_dma_uart=1;
        return 0;
    }else{
        printf("com port receive data:\n");
        for(int i = 0; i < len; i++){
            printf("%02x ",buf[i]);
            if((i+1)%8 == 0){
                printf("\n");
            }
        }
        printf("\n");
        return 0;
    }
}
static int test_driver_iic(uint8_t* buf,uint32_t len,uint8_t* buf1,uint32_t len1,uint32_t mode){

    if(mode == COM_TX || mode == COM_TX_CONTINUE){
        printf("com port com2 iic send data:\n");
        for(int i = 0; i < len; i++){
            printf("%02x ",buf[i]);
            if((i+1)%8 == 0){
                printf("\n");
            }
        }
        printf("\n");
        is_start_dma_iic=1;
        return 0;
    }else{
        printf("com port com2 iic receive data start\n");
/*
        com_cplt(com2,COM_AUTO_SIZE);//发送完成
        printf("com port com2 iic receive data end\n");
*/

        is_start_dma_iic=1;
        return 0;
    }
}
static int test_driver_spi(uint8_t* buf,uint32_t len,uint8_t* buf1,uint32_t len1,uint32_t mode){

    if(mode == COM_TX || mode == COM_TX_CONTINUE){
        printf("com port com3 spi send data:\n");
        for(int i = 0; i < len; i++){
            printf("%02x ",buf[i]);
            if((i+1)%8 == 0){
                printf("\n");
            }
        }
        printf("\n");
        is_start_dma_iic=1;
        return 0;
    }else{
        printf("com port com2 spi receive data start\n");
        is_start_dma_iic=1;
        return 0;
    }
}
/**
 * @brief 串口初始化
 * @param None
 * @retval None
 * @note 注意该函数无需用户显示调用
 */
int com_port_init(void) {
    //用户初始化
    //1 完成底层硬件初始化(如果有)
    //2 将输出输入驱动链接到com设备
    com_init_mode(com1,COM_FULL_DUPLEX_MODE);
    com_init_mode(com2,COM_MASTER_SLAVE_MODE);
    com_init_mode(com3,COM_MASTER_SLAVE_MODE);
    com_register_driver(com1,test_driver);
    com_register_driver(com2,test_driver_iic);
    com_register_driver(com3,test_driver_spi);

    //创建另一个线程，用于模拟dma
    h1 = (HANDLE)_beginthread(Thread1, 0, NULL);//线程1
    h2 = (HANDLE)_beginthread(Thread2, 0, NULL);//线程1
    h3 = (HANDLE)_beginthread(Thread3, 0, NULL);//线程1

    return com_init();
}
void Thread1(void* arg)
{
    while(1){
        while (is_start_dma_uart)
        {//开始传输
            //printf("uart start dma\n");

            is_start_dma_uart=0;

            Sleep(data_uart/8);
            is_end_dma_uart=1;//传输结束

        }
        while (is_end_dma_uart)
        {//传输结束
            //printf("uart end dma\n");

            is_end_dma_uart=0;

            com_cplt(com1,COM_AUTO_SIZE);//发送完成

        }

    }
}
void Thread2(void* arg)
{
    while(1){
        while (is_start_dma_iic)
        {//开始传输
            is_start_dma_iic=0;
            Sleep(data_iic/8);
            is_end_dma_iic=1;//传输结束
        }
        while (is_end_dma_iic)
        {//传输结束
            is_end_dma_iic=0;
            printf("com port com2 iic dma end\n");

            com_cplt(com2,COM_AUTO_SIZE);//发送完成
        }
    }
}
void Thread3(void* arg)
{
    while(1){
        while (is_start_dma_spi)
        {//开始传输
            is_start_dma_spi=0;
            Sleep(1);
            is_end_dma_spi=1;//传输结束
        }
        while (is_end_dma_spi)
        {//传输结束
            is_end_dma_spi=0;
            com_cplt(com3,COM_AUTO_SIZE);//发送完成
        }

    }
}