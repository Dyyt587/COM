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
#ifndef __SFMEM_H
#define __SFMEM_H

/************************************************************************/
/*                                                                      */
/************************************************************************/
/**
  * @brief  内存池大小
  */
#define SFMEM_POOL_SIZE         128
/**
  * @brief  内存块大小
  */
#define SFMEM_BLOCK_SIZE        4
/**
  * @brief  内存池状态表大小
  */
#define SFMEM_TABLE_SIZE        (SFMEM_POOL_SIZE/SFMEM_BLOCK_SIZE)

/**
  * @brief  内存分配管理结构体
  */
typedef struct {
    unsigned char sfmemPool[SFMEM_POOL_SIZE];      //!< 内存池
    unsigned char sfmemTable[SFMEM_TABLE_SIZE];    //!< 内存池状态表
} sfmemType;

/**
  * @brief  内存分配
  */
void *sfmemMalloc(unsigned int size);
/**
  * @brief  内存释放
  */
void sfmemFree(void *ptr);

#endif
