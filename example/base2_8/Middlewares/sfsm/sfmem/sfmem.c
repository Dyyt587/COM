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
#include <stdio.h>
#include "sfmem.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
/**
  * @brief  内存分配管理
  */
sfmemType pSfmem = {0};

/**
  * @brief      malloc函数
  * @details    申请内存
  * @param[in]  size 要申请的内存大小
  * @return     内存地址
  */
void *sfmemMalloc(unsigned int size) {
    int start = 0;
    int memCnt = 0, needCnt;
    int i, j;
    
    if(size == 0)
        return NULL;
    
    needCnt = size / SFMEM_BLOCK_SIZE;
    if(size % SFMEM_BLOCK_SIZE)
        needCnt++;
    
    for(i = 0; i < SFMEM_TABLE_SIZE; i++) {
        if(pSfmem.sfmemTable[i]) {
            memCnt = 0;
            start  = i + 1;
            continue;
        }
        
        memCnt++;
        if(memCnt == needCnt) {
            if(memCnt > 0x7f) {
                pSfmem.sfmemTable[start + 0] = ((memCnt >> 6) & 0x1f) | 0xc0;
                pSfmem.sfmemTable[start + 1] = (memCnt & 0x3f) | 0x80;
            } else {
                pSfmem.sfmemTable[start + 0] = memCnt;
                if(memCnt > 1)
                    pSfmem.sfmemTable[start + 1] = 0xff;
            }
            
            for(j = 2; j < memCnt; j++)
                pSfmem.sfmemTable[start + j] = 0xff;
            
            return (void*)(pSfmem.sfmemPool + start * SFMEM_BLOCK_SIZE);
        }
    }
    
    return NULL;
}

/**
  * @brief      free函数
  * @details    释放内存
  * @param[in]  ptr 要释放的内存地址
  * @return     无
  */
void sfmemFree(void *ptr) {
    int offset;
    int memCnt, allocCnt;
    int i;
    
    if(ptr == NULL)
        return;
    
    offset = (unsigned char *)ptr - pSfmem.sfmemPool;
    
    if(offset < SFMEM_POOL_SIZE) {
        memCnt = offset / SFMEM_BLOCK_SIZE;
        allocCnt = pSfmem.sfmemTable[memCnt];
        
        if(pSfmem.sfmemTable[memCnt] == 0xff)
            return;
        
        if(allocCnt & 0x80) {
            allocCnt = ((pSfmem.sfmemTable[memCnt] & 0x1f) << 6) | (pSfmem.sfmemTable[memCnt + 1] & 0x3f);
        } else {
            allocCnt = pSfmem.sfmemTable[memCnt];
        }
        
        for(i = 0; i < allocCnt; i++)
            pSfmem.sfmemTable[memCnt + i] = 0;
    }
    
    return;
}
