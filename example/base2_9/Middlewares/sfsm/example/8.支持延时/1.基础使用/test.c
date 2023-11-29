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
#include <unistd.h>

#include "sfsm.h"
/************************************************************************/
/*                                                                      */
/************************************************************************/
//拆分的函数
int taskFunction1(sfsmFuncParm *parm)
{
    printf("moduleFunction1\n");
    int *ret = (int *)(parm->retparm);
    *ret = 3;                               //设置延时时间3s
    return SFSM_RETURN_STATE_DELAY_FORWARD; //延后一段时间后，接下来运行下一部分
}

int taskFunction2(sfsmFuncParm *parm)
{
    printf("moduleFunction2\n");
    int *userData = (int *)(parm->userParm);
    if(*userData == 0) {
        *userData = 1;
        
        int *ret = (int *)(parm->retparm);
        *ret = 2;                       //设置延时时间2s
        return SFSM_RETURN_STATE_DELAY; //延后一段时间后再运行
    } else {
        return SFSM_RETURN_STATE_FORWARD;   //下次运行下一个函数
    }
}

int taskFunction3(sfsmFuncParm *parm)
{
    printf("moduleFunction3\n");
    return SFSM_RETURN_STATE_END;   //结束后不再运行
}

//状态表
//按照拆分函数的运行顺序填入
sfsmStateTable table[] =
{
    { taskFunction1 },
    { taskFunction2 },
    { taskFunction3 },
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
unsigned int cnt = 0;

unsigned int getTimeFunc(void) {
    return cnt;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
int main(int argc, char* argv[])
{
    //自定义变量
    int userData = 0;
    //定义状态机
    sfsmType moduleSfsm;
    //状态机注册
    sfsmRegist(&moduleSfsm, table, sizeof(table)/sizeof(sfsmStateTable), &userData);
    //设置延时相关函数
    sfsmSetDelayParm(&moduleSfsm, getTimeFunc, 20);
    
    while(1) {
        //通过该函数进行状态机的运作
        sfsmFuncRun(&moduleSfsm);
        if(sfsmGetState(&moduleSfsm) == SFSM_STATE_END) {
            printf("SFSM_STATE_END\n");
        }
        sleep(1);
        cnt++;
        cnt %= 20;
    }
    
    return 0;
}