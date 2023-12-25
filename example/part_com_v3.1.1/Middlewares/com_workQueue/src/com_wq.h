#ifndef COM_WQ_H_
#define COM_WQ_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "kfifo.h"
#include "stdio.h"
enum {
    WS_MODE_ONE_TASKING,
    WS_MODE_MULTI_TASKING,  //只有一个task fifo
    WS_MODE_NUM,//只有一个task fifo 和data fifo
};
typedef struct {
    kfifo_t *task_fifo;//任务fifo
    uint8_t is_tasking;//是否在处理任务过程中
    uint8_t type;//是否是同时仅能进行一个任务

} com_wq_t;

enum{
    WQ_SUCCESS,
    WQ_FAIL_NO_MORE_MEM,
    WQ_FAIL_NULL_POINTER,
    WQ_FAIL_NO_MORE_TASK,
    WQ_FAIL_ERROR_TASK,
    WQ_FAIL_LOCKED,

};
enum {
    WS_TASK_MODE_A,
    WS_TASK_MODE_B,
    WS_TASK_MODE_EMPTY,//用于占位任务，包证data缓冲区数据连续
};

typedef struct {
    uint32_t len;

    uint32_t addr;//任务地址
    uint8_t* pdata;//

    uint16_t mode ;
    uint16_t user_mode ;
} work_task_t;


//包括输出文件名,报错行数
//#define WS_BUG(fmt, ...) printf("%s:%d "fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define WS_BUG(fmt, ...) //printf("%s:%d "fmt, __FILE__, __LINE__, ##__VA_ARGS__)
/**
 * @brief 初始化工作调度器
 * @param ws 工作调度器
 * @param _mode 工作调度器模式
 * @param wf_a 工作fifo_a
 * @param wf_b 工作fifo_b
 * @param tf_a 任务fifo_a
 * @param tf_b 任务fifo_b
 * @param wf_a_lock 工作fifo_a锁
 * @param wf_b_lock 工作fifo_b锁
 * @param tf_a_lock 任务fifo_a锁
 * @param tf_b_lock 任务fifo_b锁
 * @return none
 */
#define wq_init(ws, _mode, wf_a,  wf_a_lock) \
    do {                                                                                      \
        kfifo_t *work_fifo = NULL;                                                          \
                                                          \
        ws.type = _mode;\
        switch (_mode) {                                                                       \
                                                                                              \
            case WS_MODE_ONE_TASKING:                                                       \
                KFIFO_INIT(work_fifo, wf_a, wf_a_lock);                                     \
                ws.task_fifo = work_fifo;                                                 \
                break;                                                                        \
            case WS_MODE_MULTI_TASKING:                                                         \
                KFIFO_INIT(work_fifo, wf_a, wf_a_lock);                                     \
                ws.task_fifo = work_fifo;                                                 \
                break;                                                                        \
            default:                                                                           \
                break;\
        }                                                                                     \
    } while (0)

#define COM_AUTO_SIZE (-1)


int com_wq_put_task(com_wq_t* wq, work_task_t *task);
int com_wq_get_task(com_wq_t* wq, work_task_t *task);
int com_wq_cplt_task(com_wq_t* wq);

//a作为用户下发任务，b作为下层反馈
#ifdef __cplusplus
}
#endif
#endif