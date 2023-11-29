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
//自定义结构体，该结构体用于在不同拆分函数中传递，并且可以用于传递最后的返回值
typedef struct {
    int a;
    int b;
    int retVal;
} userStruct;

//拆分的函数
int taskFunction1(sfsmFuncParm *parm)
{
    userStruct *userData = (userStruct *)(parm->userParm); //获取用户参数
    printf("moduleFunction1 a %d b %d\n", userData->a++, userData->b++); //使用用户参数
    
    return SFSM_RETURN_STATE_FORWARD;   //下次运行下一个函数
}

int taskFunction2(sfsmFuncParm *parm)
{
    userStruct *userData = (userStruct *)(parm->userParm); //获取用户参数
    printf("moduleFunction2 a %d b %d\n", userData->a++, userData->b++); //使用用户参数
    
    if(userData->a < 3) {
        return SFSM_RETURN_STATE_HOLD;   //下次运行当前函数
    }
    
    return SFSM_RETURN_STATE_FORWARD;   //下次运行下一个函数
}

int taskFunction3(sfsmFuncParm *parm)
{
    printf("moduleFunction3\n");
    
    userStruct *userData = (userStruct *)(parm->userParm); //获取用户参数
    if(userData->a < 4) {
        int *ret = (int *)(parm->retparm);
        *ret = 1;                           //标记要跳转的函数ID
        return SFSM_RETURN_STATE_JUMP;      //下次运行到要跳转的函数
    }
    
    userData->retVal = 10;//设置返回值
    
    return SFSM_RETURN_STATE_END;   //结束后不再运行
}

//状态表
//按照拆分函数的运行顺序填入
sfsmStateTable table[] =
{
    { taskFunction1 , 1},
    { taskFunction2 },
    { taskFunction3 },
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
int main(int argc, char* argv[])
{
    //自定义变量，该结构体用于在不同拆分函数中传递，并且可以用于传递最后的返回值
    //如果不需要也可以不定义，在注册sfsmRegist时设置为NULL即可
    userStruct userData = {0};
    //定义状态机
    sfsmType moduleSfsm;
    //状态机注册
    sfsmRegist(&moduleSfsm, table, sizeof(table)/sizeof(sfsmStateTable), &userData);
    
    while(1) {
        //通过该函数进行状态机的运作
        sfsmFuncRun(&moduleSfsm);
        //判断是否运行结束
        if(SFSM_STATE_END == sfsmGetState(&moduleSfsm)){
            printf("retval %d\n", userData.retVal);
            
            //重置后可以重新运行
            sfsmStateReset(&moduleSfsm);
        }
        sleep(1);
    }
    
    return 0;
}