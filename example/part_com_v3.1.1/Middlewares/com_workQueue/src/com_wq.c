#include "com_wq.h"
#include "stdio.h"
#include "kfifo.h"

#define __COIM_WQ_SELF_CHECK(wq)    \
    do{\
        if(wq == NULL)\
        {\
            return WQ_FAIL_NULL_POINTER;\
        }\
        if(wq->task_fifo == NULL)\
        {\
            return WQ_FAIL_NULL_POINTER;\
        }\
    }while(0)

#define __COIM_WQ_SELF_CHECK_TASK(task)    \
    do{\
        if(task == NULL)\
        {\
            return WQ_FAIL_NULL_POINTER;\
        }\
    }while(0)


int com_wq_put_task(com_wq_t* wq, work_task_t *task)
{
    //放入一个任务，并且先判断是否有空间
    //如果没有空间，返回错误
    //如果有空间，放入任务
    //如果是单任务模式，判断是否在处理任务，如果在处理任务，返回错误
    //如果不在处理任务，取出任务，放入任务
    //如果是多任务模式，判断是否在处理任务，如果在处理任务，放入任务
    //如果不在处理任务，取出任务，放入任务

    __COIM_WQ_SELF_CHECK(wq);
    __COIM_WQ_SELF_CHECK_TASK(task);

    if(kfifo_get_free(wq->task_fifo)<sizeof (work_task_t))
    {
        return WQ_FAIL_NO_MORE_MEM;
    }
    if(kfifo_put(wq->task_fifo, (uint8_t*)task, sizeof(work_task_t))==sizeof (work_task_t))
    {
        return WQ_SUCCESS;
    }
    else
    {
        return WQ_FAIL_NO_MORE_MEM;
    }
    return WQ_SUCCESS;



}
int com_wq_get_task(com_wq_t* wq, work_task_t *task)
{
    __COIM_WQ_SELF_CHECK(wq);
    __COIM_WQ_SELF_CHECK_TASK(task);
    if(kfifo_get_used(wq->task_fifo)<sizeof (work_task_t))
    {
        return WQ_FAIL_NO_MORE_TASK;
    }
    //如果是单任务模式，判断是否在处理任务，如果在处理任务，返回错误
    //如果不在处理任务，取出任务，放入任务
    if(wq->is_tasking && wq->type == WS_MODE_ONE_TASKING)
    {
        return WQ_FAIL_LOCKED;
    }
    wq->is_tasking=1;
    if(kfifo_get_try(wq->task_fifo, (uint8_t*)task, sizeof(work_task_t))==sizeof (work_task_t))
    {
        return WQ_SUCCESS;
    }
    else
    {
        return WQ_FAIL_NO_MORE_TASK;
    }


}
int com_wq_cplt_task(com_wq_t* wq)
{
    __COIM_WQ_SELF_CHECK(wq);
    wq->is_tasking=0;
    kfifo_get_index(wq->task_fifo, sizeof(work_task_t));
    return WQ_SUCCESS;
}