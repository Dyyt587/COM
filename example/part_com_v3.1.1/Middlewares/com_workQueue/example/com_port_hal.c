/**
  ******************************************************************************
  * @file           : com_port.c
  * @author         : 80520
  * @brief          : None
  * @attention      : None
  * @date           : 2023/12/3
  ******************************************************************************
  */
#include "com_port_hal.h"
#include "cmsis_os2.h"
#include "usart.h"
#include "elog.h"
#include <stdio.h>

static int test_driver(uint8_t *buf, uint32_t len,uint8_t *rxbuf, uint32_t rxlen, uint32_t mode) {
    //将数据打印出来，8个数据换行一次
    //    printf("com port receive data:\n");
    //    for(int i = 0; i < len; i++){
    //        printf("%02x ",buf[i]);
    //        if((i+1)%8 == 0){
    //            printf("\n");
    //        }
    //    }
    //HAL_UART_Transmit(&huart1,buf,len,10000);
    if (mode == COM_TX || mode == COM_TX_CONTINUE) {
        HAL_UART_Transmit_IT(&huart1, buf, len);
    } else {
        //printf("com port receive data \n");
    }
    //完成任务
    return 0;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    com_cplt(com1,COM_AUTO_SIZE);
}
static volatile int32_t last_size = 0;
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {

    printf("ee");
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart == &huart1) {
        /**           HAL_UART_RXEVENT_TC                 = 0x00U,
      *           HAL_UART_RXEVENT_HT                 = 0x01U,
      *           HAL_UART_RXEVENT_IDLE               = 0x02U,*/
        HAL_UART_RxEventTypeTypeDef rxEvent = HAL_UARTEx_GetRxEventType(huart);


        if (rxEvent == HAL_UART_RXEVENT_IDLE || rxEvent == HAL_UART_RXEVENT_TC) {
            int32_t tmp;
            if (Size >= last_size) {
                tmp = Size - last_size;
                //log_d("com1 %d\n", com_get_rxbuf_size(com1));
            } else {
                tmp = last_size + com_get_rxbuf_size(com1) - Size;
            }
            //log_d("last_size %d", last_size);

            last_size = Size;

            //更新cache
            //SCB_InvalidateDCache_by_Addr(com_get_rxbuf_in(com1), tmp/4);
            int32_t tmp_bytes = tmp;              // 将tmp转换为整数类型
            int32_t tmp_words = tmp_bytes / 4;    // 以4字节为单位计算tmp的整数商
            int32_t tmp_remainder = tmp_bytes % 4;// 计算tmp除以4的余数

            if (tmp_remainder > 0) {
                tmp_words++;// 如果余数大于0，则向上取整
            }

            SCB_InvalidateDCache_by_Addr(com_get_rxbuf_in(com1), tmp_words);// 使用向上取整后的值进行操作
            com_received(com1, tmp);
        }
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
    int ret = 0;
    static uint8_t buf[8];
    ret = com_init();
    com_register_driver(com1, test_driver);
    com_register_driver(com2, test_driver);
    com_register_driver(com3, test_driver);
    //com_receive(com1,1024);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, com_get_rxbuf_addr(com1), com_get_rxbuf_size(com1));//启动接收了
    //HAL_UARTEx_ReceiveToIdle_DMA(&huart1, buf, sizeof(buf));
    return ret;
}

void StartDefaultTask(void *argument) {
    uint8_t data;
    uint8_t buf[] = "1234567890\n";
    //com_send(com1, buf, 12);
    //com_send(com1, buf, sizeof(buf));
    log_d("com1 size %d", com_get_rxbuf_size(com1));
    log_d("com1 addr 0x%x", com_get_rxbuf_addr(com2));
    /* Infinite loop */
    for (;;) {
        //			com_send(com1, buf, sizeof(buf));
        //com_send(com1, buf, sizeof(buf));
        //com_send(com1, buf, 12);
        //   log_d("com1 addr %d", com_get_rxbuf_addr(com2));

        //printf("test\n");
        osDelay(100);
    }
}