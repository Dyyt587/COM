/*
 * Copyright (c) 2022 Hong Jiahua
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef __SFSM_H__
#define __SFSM_H__

#include "sfsm_config.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
/**
  * @brief  函数状态类型
  */
#if SFSM_FUNC_STATE_SUPPORT
typedef enum {
    SFSM_FUNC_STATE_NORMAL   = 0,   //!< 函数状态正常
    SFSM_FUNC_STATE_ABANDON,        //!< 函数不运行
    SFSM_FUNC_STATE_MAX
} sfsmFuncState;
#endif

/**
  * @brief  状态机状态类型
  */
typedef enum {
    SFSM_STATE_NORMAL   = 0,    //!< 状态机状态正常
    SFSM_STATE_END,             //!< 状态机运行结束
    SFSM_STATE_MAX
} sfsmState;

/**
  * @brief  返回值类型
  */
typedef enum {
    SFSM_RETURN_STATE_FORWARD = 0,      //!< 运行下一部分
    SFSM_RETURN_STATE_HOLD,             //!< 保持运行当前部分
    SFSM_RETURN_STATE_JUMP,             //!< 跳转到某一部分
    SFSM_RETURN_STATE_END,              //!< 不再运行
    SFSM_RETURN_STATE_RESTART,          //!< 重头开始
#if SFSM_DELAY
    SFSM_RETURN_STATE_DELAY,            //!< 延后一段时间后再运行
    SFSM_RETURN_STATE_DELAY_FORWARD,    //!< 延后一段时间后，接下来运行下一部分
#endif
#if SFSM_FUNC_STATE_SUPPORT
    SFSM_RETURN_STATE_ABANDON_FORWARD,   //!< 当前函数不再运行，接下来运行下一部分
    SFSM_RETURN_STATE_ABANDON_JUMP,      //!< 当前函数不再运行，跳转到某一部分
#endif
    SFSM_RETURN_STATE_MAX
} sfsmReturnState;

/**
  * @brief  函数参数类型
  */
typedef struct {
    //默认自带
    void *retparm;          //!< 用于设置一些系统参数
    
    //自定义支持
    void *userParm;         //!< 用户自定义参数
#if SFSM_LOCAL_PARM
    void *localParm;        //!< 局部参数，生命周期维持在整个流程
    void *partLocalParm;    //!< 局部参数，生命周期维持在单个函数
#endif
#if SFSM_DEFAULT_FLAG_PARM
    void *flagParm;         //!< 标记参数
#endif
} sfsmFuncParm;

/**
  * @brief  状态表类型
  */
typedef struct {
    int (*function)(sfsmFuncParm *parm);    //!< 函数指针
    int jumpId;                             //!< 跳转id
#if SFSM_DEFAULT_FLAG_PARM
    void *flagParm;                         //!< 默认标记
#endif
#if SFSM_FUNC_STATE_SUPPORT
    int curState;                           //!< 当前状态
#endif
} sfsmStateTable;

/**
  * @brief  状态机参数类型
  */
typedef struct {
    int curState;                       //!< 当前状态
    int funcId;                         //!< 函数ID
    sfsmStateTable * stateTable;        //!< 状态表
    int size;                           //!< 表的项数
    void *userParm;                     //!< 传递参数
    int retParm;                        //!< 返回参数
#if SFSM_LOCAL_PARM
    void *localParmAddr;                //!< 临时参数地址, 生命周期维持在整个状态机
    int localParmSize;                  //!< 临时参数大小
    
    void *partLocalParmAddr;            //!< 临时参数地址, 生命周期维持在单个函数
    int partLocalParmSize;              //!< 临时参数大小
#endif
#if SFSM_SETUP_FUNC_SUPPORT
    int setupFlag;                      //!< setup flag
    void (*setupFunc)(void *parm);      //!< setup callback
#endif
#if SFSM_TEARDOWN_FUNC_SUPPORT
    int teardownFlag;                   //!< teardown flag
    void (*teardownFunc)(void *parm);   //!< teardown callback
#endif
#if SFSM_DELAY
    int delayFlag;                      //!< 延迟标示
    unsigned int startTime;             //!< 设置延时时的时间
    unsigned int delayTime;             //!< 设置延时的时间
    unsigned int timeMaxCnt;            //!< 时间最大值
    unsigned int (*getTimeFunc)(void);  //!< 获取时间的回调函数
#endif
}sfsmType;

/*状态机注册*/
void sfsmRegist(sfsmType* pSfsm, sfsmStateTable* pTable, int size, void *userParm);
/*状态机参数重置*/
void sfsmStateReset(sfsmType* pSfsm);
/*获取状态机当前状态*/
int sfsmGetState(sfsmType* pSfsm);
/*设置Setup函数*/
void sfsmSetSetupFunc(sfsmType* pSfsm, void (*setupFunc)(void *));
/*设置Teardown函数*/
void sfsmSetTeardownFunc(sfsmType* pSfsm, void (*teardownFunc)(void *));
/*设置临时参数大小*/
void sfsmSetLocalParmSize(sfsmType* pSfsm, int localParmSize, int partLocalParmSize);
/*设置延时参数*/
void sfsmSetDelayParm(sfsmType* pSfsm, unsigned int (*getTimeFunc)(void), unsigned int timeMaxCnt);
/*函数处理*/
int sfsmFuncRun(sfsmType* pSfsm);

#endif
