/**
  ******************************************************************************
  * @file           : com_config.h
  * @author         : 80520
  * @brief          : None
  * @attention      : None
  * @date           : 2023/12/8
  ******************************************************************************
  */

#ifndef WORK_SCHEDULER_COM_CONFIG_H
#define WORK_SCHEDULER_COM_CONFIG_H
#ifdef __cplusplus
extern "C" {
#endif
//TODO:使用port init com 不要反了
//配置项说明: 设备名(id),发送数据缓存大小,接收数据缓存大小,发送任务缓存大小,接收任务缓存大小,
//                                  发送数据缓存锁,接收数据缓存锁,发送任务缓存锁,接收任务缓存锁


#define COM_DEVICE_NAME \
        com1,           \
        com2,           \
        com3,           \

//com mode tx_task fifo  rx_task fifo
#define COM_DEVICE_TABLE(){ \
        COM_DEVICE_CFG(com1, WS_MODE_ONE_TASKING, 4096,4096,    4096,4096); \
        COM_DEVICE_CFG(com2, WS_MODE_ONE_TASKING, 1024,1024,    1024,1024); \
        COM_DEVICE_CFG(com3, WS_MODE_ONE_TASKING, 1024,1024,    1024,1024); \
}

#ifdef __cplusplus
}
#endif
#endif//WORK_SCHEDULER_COM_CONFIG_H
