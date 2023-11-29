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
#ifndef __SFSM_CONFIG_H__
#define __SFSM_CONFIG_H__


extern unsigned int getTimeFunc(void);
/************************************************************************/
/*                                                                      */
/************************************************************************/
/**
  * @brief  每个函数都有状态支持 1：开启 0：关闭
  */
#define SFSM_FUNC_STATE_SUPPORT     0
/**
  * @brief  支持Setup函数 1：开启 0：关闭
  */
#define SFSM_SETUP_FUNC_SUPPORT     0
/**
  * @brief  支持Teardown函数 1：开启 0：关闭
  */
#define SFSM_TEARDOWN_FUNC_SUPPORT  0
/**
  * @brief  支持自动重载 1：开启 0：关闭
  */
#define SFSM_AUTO_RELOAD            0
/**
  * @brief  支持局部参数加载 1：开启 0：关闭
  */
#define SFSM_LOCAL_PARM             0

/**
  * @brief  局部参数默认分配内存大小，生命周期维持在整个状态机，只有在 SFSM_LOCAL_PARM 开启时才有效
  */
#define SFSM_LOCAL_PARM_SIZE        0

/**
  * @brief  局部参数默认分配内存大小，生命周期维持在单个函数，只有在 SFSM_LOCAL_PARM 开启时才有效
  */
#define SFSM_PART_LOCAL_PARM_SIZE   0

/**
  * @brief  局部参数内存申请，只有在 SFSM_LOCAL_PARM 开启时才有效
  */
#define SFSM_MALLOC                 malloc

/**
  * @brief  局部参数内存释放，只有在 SFSM_LOCAL_PARM 开启时才有效
  */
#define SFSM_FREE                   free

/**
  * @brief  每个函数都有默认标记 1：开启 0：关闭
  */
#define SFSM_DEFAULT_FLAG_PARM      0

/**
  * @brief  支持延迟调用功能 1：开启 0：关闭
  */
#define SFSM_DELAY                  1

/**
  * @brief  设置默认的获取时间函数，只有在 SFSM_DELAY 开启时才有效
  */
#define SFSM_GET_TIME_FUNC          getTimeFunc

/**
  * @brief  设置默认的时间最大值，只有在 SFSM_DELAY 开启时才有效
  */
#define SFSM_TIME_MAX_CNT           20

#endif