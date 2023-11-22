
/*
 * Auto generated Run-Time-Environment Configuration File
 *      *** Do not modify ! ***
 *
 * Project: 'STM32H7B0' 
 * Target:  'STM32H7B0' 
 */

#ifndef PRE_INCLUDE_GLOBAL_H
#define PRE_INCLUDE_GLOBAL_H

/* GorgonMeducer::Utilities:perf_counter:Benchmark:Coremark:1.1.1 */
//! \brief Enable Coremark
#define __PERF_COUNTER_COREMARK__   1
#define MAIN_HAS_NOARGC             1
#define MAIN_HAS_NORETURN           1
#define HAS_FLOAT                   1
/* GorgonMeducer::Utilities:perf_counter:Core:Source:2.2.3 */
#define __PERF_COUNTER_CFG_USE_SYSTICK_WRAPPER__ 1
/* GorgonMeducer::Utilities:perf_counter:FreeRTOS Patch:2.2.3 */
//! \brief Enable RTOS Patch for perf_counter
#define __PERF_CNT_USE_RTOS__ 1
            
#define traceTASK_SWITCHED_OUT_DISABLE  
#define traceTASK_SWITCHED_IN_DISABLE

extern void __freertos_evr_on_task_switched_out (void *ptTCB);
extern void __freertos_evr_on_task_switched_in(void *ptTCB, unsigned int uxTopPriority) ;

#   define traceTASK_SWITCHED_OUT()                                             \
        __freertos_evr_on_task_switched_out(pxCurrentTCB)
#   define traceTASK_SWITCHED_IN()                                              \
        __freertos_evr_on_task_switched_in(pxCurrentTCB, uxTopReadyPriority)


#endif /* PRE_INCLUDE_GLOBAL_H */
