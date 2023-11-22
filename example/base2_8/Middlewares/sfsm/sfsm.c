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
 /**
 * @file        sfsm.c
 * @brief       sfsm
 * @details     sfsm
 * @author      Hong Jiahua
 * @version     1.2.0
 * @date        2022-02-09
 * @copyright   MIT License
 */
#include <string.h>
#include "sfsm.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
/**
  * @brief      状态机注册
  * @details    状态机注册函数，用于初始化状态机
  * @param[in]  pSfsm       状态机
  * @param[in]  pTable      状态表
  * @param[in]  size        状态表的长度
  * @param[in]  userParm    状态机内部传递用的参数
  * @return     无
  * @retval     无
  */
void sfsmRegist(sfsmType* pSfsm, sfsmStateTable* pTable, int size, void *userParm)
{
    pSfsm->curState     = SFSM_STATE_NORMAL;
    pSfsm->funcId       = 0;
    pSfsm->stateTable   = pTable;
    pSfsm->size         = size;
    pSfsm->userParm     = userParm;
    pSfsm->retParm      = 0;
#if SFSM_FUNC_STATE_SUPPORT
    int i;
    for(i = 0; i < pSfsm->size; i++)
        pSfsm->stateTable[i].curState = SFSM_FUNC_STATE_NORMAL;
#endif

#if SFSM_SETUP_FUNC_SUPPORT
    pSfsm->setupFlag    = 1;
    pSfsm->setupFunc    = NULL;
#endif

#if SFSM_TEARDOWN_FUNC_SUPPORT
    pSfsm->teardownFlag = 1;
    pSfsm->teardownFunc = NULL;
#endif

#if SFSM_LOCAL_PARM
    pSfsm->localParmAddr        = NULL;
    pSfsm->localParmSize        = SFSM_LOCAL_PARM_SIZE;
    
    pSfsm->partLocalParmAddr    = NULL;
    pSfsm->partLocalParmSize    = SFSM_PART_LOCAL_PARM_SIZE;
#endif

#if SFSM_DELAY
    pSfsm->delayFlag    = 0;
    pSfsm->startTime    = 0;
    pSfsm->delayTime    = 0;
    pSfsm->timeMaxCnt   = SFSM_TIME_MAX_CNT;
    pSfsm->getTimeFunc  = SFSM_GET_TIME_FUNC;
#endif
}

/**
  * @brief      状态机参数重置
  * @details    用于将参数重置回初始化状态
  * @param[in]  pSfsm   状态机
  * @return     无
  * @retval     无
  */
void sfsmStateReset(sfsmType* pSfsm)
{
    pSfsm->curState     = SFSM_STATE_NORMAL;
    pSfsm->funcId       = 0;
    pSfsm->retParm      = 0;
#if SFSM_FUNC_STATE_SUPPORT
    int i;
    for(i = 0; i < pSfsm->size; i++)
        pSfsm->stateTable[i].curState = SFSM_FUNC_STATE_NORMAL;
#endif

#if SFSM_SETUP_FUNC_SUPPORT
    if(pSfsm->setupFunc)
        pSfsm->setupFlag    = 0;
    else
        pSfsm->setupFlag    = 1;
#endif

#if SFSM_TEARDOWN_FUNC_SUPPORT
    if(pSfsm->teardownFunc)
        pSfsm->teardownFlag = 0;
    else
        pSfsm->teardownFlag = 1;
#endif

#if SFSM_LOCAL_PARM
    if(pSfsm->localParmSize != 0) {
        if(pSfsm->localParmAddr)
            SFSM_FREE(pSfsm->localParmAddr);
        pSfsm->localParmAddr = SFSM_MALLOC(pSfsm->localParmSize);
        memset(pSfsm->localParmAddr, 0, pSfsm->localParmSize);
    }
    
    if(pSfsm->partLocalParmSize != 0) {
        if(pSfsm->partLocalParmAddr)
            SFSM_FREE(pSfsm->partLocalParmAddr);
        pSfsm->partLocalParmAddr = SFSM_MALLOC(pSfsm->partLocalParmSize);
        memset(pSfsm->partLocalParmAddr, 0, pSfsm->partLocalParmSize);
    }
#endif

#if SFSM_DELAY
    pSfsm->delayFlag    = 0;
#endif
}

/**
  * @brief      状态机状态获取
  * @details    用于获取当前状态机状态
  * @param[in]  pSfsm   状态机
  *
  * @return     sfsmState
  * @retval     SFSM_STATE_NORMAL   状态机正常
  * @retval     SFSM_STATE_END      状态机运行结束
  */
int sfsmGetState(sfsmType* pSfsm)
{
    return pSfsm->curState;
}

/**
  * @brief      设置Setup函数
  * @details    设置Setup函数
  * @param[in]  pSfsm       状态机
  * @param[in]  setupFunc   Setup函数
  * @return     无
  */
void sfsmSetSetupFunc(sfsmType* pSfsm, void (*setupFunc)(void *))
{
#if SFSM_SETUP_FUNC_SUPPORT
    pSfsm->setupFunc = setupFunc;
    pSfsm->setupFlag = 0;
#endif
}

/**
  * @brief      设置Teardown函数
  * @details    设置Teardown函数
  * @param[in]  pSfsm       状态机
  * @param[in]  setupFunc   Teardown函数
  * @return     无
  */
void sfsmSetTeardownFunc(sfsmType* pSfsm, void (*teardownFunc)(void *))
{
#if SFSM_TEARDOWN_FUNC_SUPPORT
    pSfsm->teardownFunc = teardownFunc;
    pSfsm->teardownFlag = 0;
#endif
}

/**
  * @brief      设置临时参数大小
  * @details    设置临时参数大小
  * @param[in]  pSfsm       状态机
  * @param[in]  size        临时参数大小
  * @return     无
  */
void sfsmSetLocalParmSize(sfsmType* pSfsm, int localParmSize, int partLocalParmSize)
{
#if SFSM_LOCAL_PARM
    pSfsm->localParmSize        = localParmSize;
    pSfsm->partLocalParmSize    = partLocalParmSize;
#endif
}

/**
  * @brief      设置延时参数
  * @details    设置延时参数
  * @param[in]  pSfsm       状态机
  * @param[in]  getTimeFunc 获取时间的函数
  * @param[in]  timeMaxCnt  时间最大值
  * @return     无
  */
void sfsmSetDelayParm(sfsmType* pSfsm, unsigned int (*getTimeFunc)(void), unsigned int timeMaxCnt)
{
#if SFSM_DELAY
    pSfsm->delayFlag    = 0;
    pSfsm->timeMaxCnt   = timeMaxCnt;
    pSfsm->getTimeFunc  = getTimeFunc;
#endif
}

/**
  * @brief      状态机运行函数
  * @details    状态机运行函数
  * @param[in]  pSfsm       状态机
  *
  * @return     sfsmState
  * @retval     SFSM_STATE_NORMAL   状态机正常
  * @retval     SFSM_STATE_END      状态机运行结束
  */
int sfsmFuncRun(sfsmType* pSfsm)
{
    sfsmStateTable* pStateTable = pSfsm->stateTable;
    
    if(pSfsm->curState == SFSM_STATE_END) {
#if SFSM_AUTO_RELOAD
        sfsmStateReset(pSfsm);
#endif
        return pSfsm->curState;
    }
    
#if SFSM_DELAY
    if(pSfsm->delayFlag) {
        if(pSfsm->getTimeFunc) {
            unsigned int time = pSfsm->getTimeFunc();
            
            if(time >= pSfsm->startTime) {
                if((time - pSfsm->startTime) < pSfsm->delayTime)
                    return pSfsm->curState;
            } else {
                if((pSfsm->timeMaxCnt - pSfsm->startTime + time) < pSfsm->delayTime)
                    return pSfsm->curState;
            }
        }
        pSfsm->delayFlag = 0;
    }
#endif

#if SFSM_SETUP_FUNC_SUPPORT
    if(pSfsm->setupFlag == 0) {
        if(pSfsm->setupFunc)
            pSfsm->setupFunc(pSfsm->userParm);
        pSfsm->setupFlag = 1;
    }
#endif

#if SFSM_LOCAL_PARM
    if(pSfsm->localParmSize != 0 && pSfsm->localParmAddr == NULL) {
        pSfsm->localParmAddr = SFSM_MALLOC(pSfsm->localParmSize);
        memset(pSfsm->localParmAddr, 0, pSfsm->localParmSize);
    }
    
    if(pSfsm->partLocalParmSize != 0 && pSfsm->partLocalParmAddr == NULL) {
        pSfsm->partLocalParmAddr = SFSM_MALLOC(pSfsm->partLocalParmSize);
        memset(pSfsm->partLocalParmAddr, 0, pSfsm->partLocalParmSize);
    }
#endif

#if SFSM_FUNC_STATE_SUPPORT
    if(pStateTable[pSfsm->funcId].curState == SFSM_FUNC_STATE_ABANDON) {
        int tempId = pSfsm->funcId + 1;
        while(tempId < pSfsm->size) {
            if(pStateTable[tempId].curState == SFSM_FUNC_STATE_ABANDON) {
                tempId++;
                continue;
            }
            
            pSfsm->funcId = tempId;
            break;
        }
        
        if(tempId >= pSfsm->size) {
            pSfsm->funcId = 0;
            pSfsm->curState = SFSM_STATE_END;
            return pSfsm->curState;
        }
    }
#endif
    if (pStateTable[pSfsm->funcId].function) {
        sfsmFuncParm parm = {0};
        
        parm.retparm        = &(pSfsm->retParm);
        parm.userParm       = pSfsm->userParm;
        
        #if SFSM_LOCAL_PARM
        parm.localParm      = pSfsm->localParmAddr;
        parm.partLocalParm  = pSfsm->partLocalParmAddr;
        #endif
        
        #if SFSM_DEFAULT_FLAG_PARM
        parm.flagParm       = pStateTable[pSfsm->funcId].flagParm;
        #endif
        
        int ret = pStateTable[pSfsm->funcId].function(&parm);
        switch(ret)
        {
            case SFSM_RETURN_STATE_FORWARD:
            {
                if((pSfsm->funcId + 1) < pSfsm->size)
                    pSfsm->funcId += 1;
                else {
                    pSfsm->funcId = 0;
                    pSfsm->curState = SFSM_STATE_END;
                }
                
                #if SFSM_LOCAL_PARM
                if(pSfsm->partLocalParmSize != 0 && pSfsm->partLocalParmAddr != NULL)
                    memset(pSfsm->partLocalParmAddr, 0, pSfsm->partLocalParmSize);
                #endif
            }
            break;
            
            case SFSM_RETURN_STATE_HOLD:
            break;
            
            case SFSM_RETURN_STATE_JUMP:
            {
                int i;
                
                for(i = 0; i < pSfsm->size; i++) {
                    if(pStateTable[i].jumpId == pSfsm->retParm)
                        pSfsm->funcId = i;
                }
                
                #if SFSM_LOCAL_PARM
                if(pSfsm->partLocalParmSize != 0 && pSfsm->partLocalParmAddr != NULL)
                    memset(pSfsm->partLocalParmAddr, 0, pSfsm->partLocalParmSize);
                #endif
            }
            break;
            
            case SFSM_RETURN_STATE_END:
            {
                pSfsm->curState = SFSM_STATE_END;
                pSfsm->funcId = 0;
                
                #if SFSM_LOCAL_PARM
                if(pSfsm->localParmSize != 0 && pSfsm->localParmAddr != NULL) {
                    SFSM_FREE(pSfsm->localParmAddr);
                    pSfsm->localParmAddr = NULL;
                }
                
                if(pSfsm->partLocalParmSize != 0 && pSfsm->partLocalParmAddr != NULL) {
                    SFSM_FREE(pSfsm->partLocalParmAddr);
                    pSfsm->partLocalParmAddr = NULL;
                }
                #endif
                
                #if SFSM_TEARDOWN_FUNC_SUPPORT
                if(pSfsm->teardownFlag == 0) {
                    if(pSfsm->teardownFunc)
                        pSfsm->teardownFunc(pSfsm->userParm);
                    pSfsm->teardownFlag = 1;
                }
                #endif
            }
            break;
            
            case SFSM_RETURN_STATE_RESTART:
            {
                pSfsm->funcId = 0;
                
                #if SFSM_LOCAL_PARM
                if(pSfsm->localParmSize != 0 && pSfsm->localParmAddr != NULL)
                    memset(pSfsm->localParmAddr, 0, pSfsm->localParmSize);
                
                if(pSfsm->partLocalParmSize != 0 && pSfsm->partLocalParmAddr != NULL)
                    memset(pSfsm->partLocalParmAddr, 0, pSfsm->partLocalParmSize);
                #endif
            }
            break;
            
#if SFSM_DELAY
            case SFSM_RETURN_STATE_DELAY:
            {
                if(pSfsm->getTimeFunc) {
                    pSfsm->startTime = pSfsm->getTimeFunc();
                    pSfsm->delayTime = pSfsm->retParm;
                    pSfsm->delayFlag = 1;
                }
            }
            break;
            
            case SFSM_RETURN_STATE_DELAY_FORWARD:
            {
                if(pSfsm->getTimeFunc) {
                    pSfsm->startTime = pSfsm->getTimeFunc();
                    pSfsm->delayTime = pSfsm->retParm;
                    pSfsm->delayFlag = 1;
                }
                
                if((pSfsm->funcId + 1) < pSfsm->size)
                    pSfsm->funcId += 1;
                else {
                    pSfsm->funcId = 0;
                    pSfsm->curState = SFSM_STATE_END;
                }
                
                #if SFSM_LOCAL_PARM
                if(pSfsm->partLocalParmSize != 0 && pSfsm->partLocalParmAddr != NULL)
                    memset(pSfsm->partLocalParmAddr, 0, pSfsm->partLocalParmSize);
                #endif
            }
            break;
#endif
            
#if SFSM_FUNC_STATE_SUPPORT
            case SFSM_RETURN_STATE_ABANDON_FORWARD:
            {
                pStateTable[pSfsm->funcId].curState = SFSM_FUNC_STATE_ABANDON;
                
                if((pSfsm->funcId + 1) < pSfsm->size)
                    pSfsm->funcId += 1;
                else {
                    pSfsm->funcId = 0;
                    pSfsm->curState = SFSM_STATE_END;
                }
                
                #if SFSM_LOCAL_PARM
                if(pSfsm->partLocalParmSize != 0 && pSfsm->partLocalParmAddr != NULL)
                    memset(pSfsm->partLocalParmAddr, 0, pSfsm->partLocalParmSize);
                #endif
            }
            break;
            
            case SFSM_RETURN_STATE_ABANDON_JUMP:
            {
                pStateTable[pSfsm->funcId].curState = SFSM_FUNC_STATE_ABANDON;
                
                int i;
                for(i = 0; i < pSfsm->size; i++) {
                    if(pStateTable[i].jumpId == pSfsm->retParm)
                        pSfsm->funcId = i;
                }
                
                #if SFSM_LOCAL_PARM
                if(pSfsm->partLocalParmSize != 0 && pSfsm->partLocalParmAddr != NULL)
                    memset(pSfsm->partLocalParmAddr, 0, pSfsm->partLocalParmSize);
                #endif
            }
            break;
#endif
            
            default:
            break;
        }
    }
    
    return pSfsm->curState;
}
