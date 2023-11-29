# sfsm

## 介绍

sfsm是一个用于拆分函数的状态机。主要适用于无OS的MCU上运行。在该框架下可以方便快捷地拆分函数，并且支持多级嵌套。

## 使用说明

只需要包含sfsm.c、sfsm.h和sfsm_config.h三个文件即可使用。

## 基础场景示例

假设程序中有一个`moduleTask()`的任务需要轮询，这个函数的耗时为20ms。但是循环中有其他的任务需要至少每10ms运行一次，这个时候就需要拆分我们`moduleTask()`这个函数来保证小于10ms就要退出，将CPU让给其他任务。

```c
void moduleTask(void) {
    moduleFunction1();
    moduleFunction2();
    moduleFunction3();
    /*...*/
}

int main(int argc, char* argv[])
{
    /*...*/
    while(1) {
        moduleTask();
        /*...*/
        OtherTask();
        /*其他任务*/
    }
}
```

首先我们要拆分函数，拆分函数的类型为`int (*function)(sfsmFuncParm *parm)`.

假设将原本的`moduleTask()`拆分成3个部分，拆分为如下代码：

```c
//拆分的函数
int taskFunction1(sfsmFuncParm *parm)
{
    moduleFunction1();

    return SFSM_RETURN_STATE_FORWARD;   //下次运行下一个函数
}

int taskFunction2(sfsmFuncParm *parm)
{
    moduleFunction2();

    return SFSM_RETURN_STATE_FORWARD;   //下次运行下一个函数
}

int taskFunction3(sfsmFuncParm *parm)
{
    moduleFunction3();

    return SFSM_RETURN_STATE_RESTART;   //下次重头开始
}
```

然后定义状态表，将刚才拆分的函数放进去。

```c
sfsmStateTable table[] =
{
    { taskFunction1 },
    { taskFunction2 },
    { taskFunction3 },
};
```

接着需要定义并初始化状态机：

```c
//定义状态机
sfsmType moduleSfsm;
//状态机注册
sfsmRegist(&moduleSfsm, table, sizeof(table)/sizeof(sfsmStateTable), NULL);
```

最后只要调用`sfsmFuncRun(&moduleSfsm);`就可以运行这个状态机了。

```c
sfsmFuncRun(&moduleSfsm);
```

放入主循环中：

```c
int main(int argc, char* argv[])
{
    /*...*/
    while(1) {
        sfsmFuncRun(&moduleSfsm);
        /*...*/
        OtherTask();
        /*其他任务*/
    }
}
```

## 接口参数说明

### 返回值

状态机就是通过拆分函数的返回值来进行驱动，所以返回值的设定很重要。

```c
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
```

### 状态机状态

如果是一直循环运行，则状态一直为SFSM_STATE_NORMAL；如果是一次性运行，则运行结束后状态变为SFSM_STATE_END。

```c
typedef enum {
    SFSM_STATE_NORMAL   = 0,    //!< 状态机状态正常
    SFSM_STATE_END,             //!< 状态机运行结束
    SFSM_STATE_MAX
} sfsmState;
```

### 函数参数

拆分函数的类型为`int (*function)(sfsmFuncParm *parm)`，带有的函数参数类型如下：

```c
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
```

### 功能函数

状态机支持的一些功能函数：

```c
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
```

## 目录结构

代码目录结构如下：

```
sfsm
├── config
│   └── sfsm_config.h
├── sfmem
│   ├── sfmem.c
│   └── sfmem.h
├── example
│   ├── 1.基本示例
│   ├── 2.基本功能
│   ├── 3.支持函数状态
│   ├── 4.支持Setup和Teardown函数
│   ├── 5.支持自动重载
│   ├── 6.支持局部参数加载
│   ├── 7.支持函数标记
│   ├── 8.支持延时
│   ├── 9.多级嵌套
│   └── 10.组合应用
├── sfsm.c
└── sfsm.h
```

只需要包含sfsm.c、sfsm.h和sfsm_config.h三个文件即可使用。

example目录里提供一些示例程序，这些示例程序可以在gcc下直接使用make编译使用。

## 基本功能

### 单次运行

如果状态机只需要运行一次，则拆分函数的返回值设置为`SFSM_RETURN_STATE_END`。

```c
int taskFunction3(sfsmFuncParm *parm)
{
    moduleFunction3();

    return SFSM_RETURN_STATE_END;   //结束后不再运行
}
```

返回状态`SFSM_RETURN_STATE_END`之后，再调用`sfsmFuncRun`的话无作用，不会运行。

### 获取状态

单次运行时，需要知道当前是否运行结束，可以调用接口`sfsmGetState`进行状态获取。

如果返回值为`SFSM_STATE_END`则表示运行结束。

```c
int sfsmGetState(sfsmType* pSfsm);
```

例如：

```c
if(sfsmGetState(&moduleSfsm) == SFSM_STATE_END) {
    printf("SFSM_STATE_END\n");
}
```

也可以在状态机运行中直接通过获取`sfsmFuncRun`的返回值来获取当前的状态，例如：

```c
if(sfsmFuncRun(&moduleSfsm) == SFSM_STATE_END) {
    printf("SFSM_STATE_END\n");
}
```

### 重新运行

如果还需要再运行，可以调用接口`sfsmStateReset`进行重置，之后再调用`sfsmFuncRun`就会重新运行。

```c
void sfsmStateReset(sfsmType* pSfsm);
```

### 函数保持

如果在某个函数运行结束后，下一次还要进到该函数，可以将返回值设置为`SFSM_RETURN_STATE_HOLD`。

```c
int taskFunction(sfsmFuncParm *parm)
{
    int ret = moduleFunction2();
    if(ret < 0)
        return SFSM_RETURN_STATE_HOLD;   //下次运行当前函数
    else
        return SFSM_RETURN_STATE_FORWARD;   //下次运行下一个函数
}
```

该功能适合某些需要满足条件才能继续运行下去的函数，例如延时。

### 函数跳转

如果在某个函数运行结束后，接下来不运行到下一个函数，而是要跳转到某个函数去，可以将返回值设置为`SFSM_RETURN_STATE_JUMP`， 并设置要跳转的函数ID，同时在定义状态表的位置定义那个函数ID。注意，函数ID必须大于0。

假设要跳转到`taskFunction1`，将`taskFunction1`对应的函数ID设置为1，状态表定义如下：

```c
sfsmStateTable table[] =
{
    { taskFunction1 , 1 },
    { taskFunction2 },
    { taskFunction3 },
};
```

然后在函数中设置跳转ID和返回值：

```c
int taskFunction(sfsmFuncParm *parm)
{
    int ret = moduleFunction2();
    if(ret < 0) {
        int *ret = (int *)(parm->retparm);
        *ret = 1;                           //标记要跳转的函数ID
        return SFSM_RETURN_STATE_JUMP;      //下次运行到要跳转的函数
    } else
        return SFSM_RETURN_STATE_FORWARD;   //下次运行下一个函数
}
```

### 用户参数

可以自定义用户参数在不同的拆分函数中传递，例如定义了一个变量：

```c
typedef struct {
    int a;
    int b;
    int retVal;
} userStruct;

userStruct userData;
```

需要在初始化的时候将变量设置进去：

```c
sfsmRegist(&moduleSfsm, tabl, sizeof(table)/sizeof(sfsmStateTable), (void *)&userData);
```

然后就可以在函数中使用：

```c
int taskFunction(sfsmFuncParm *parm)
{
    userStruct *userData = (userStruct *)(parm->userParm);
    moduleFunction2(userData->a, userData->b); //使用用户参数
    return SFSM_RETURN_STATE_FORWARD;   //下次运行下一个函数
}
```

### 函数返回值

函数如果有返回值需要返回给外部，可以使用上一小节的用户参数进行返回。

例如：

```c
int taskFunction(sfsmFuncParm *parm)
{
    userStruct *userData = (userStruct *)(parm->userParm);
    userData->retVal = moduleFunction2(userData->a, userData->b); //使用用户参数
    return SFSM_RETURN_STATE_END;   //结束后不再运行
}
```

外部在检测到状态机运行结束时，可以去获取返回值

```c
if(sfsmGetState(&moduleSfsm) == SFSM_STATE_END){
    printf("ret %d\n", userData.retVal);    //打印返回值
}
```

## 高级功能

下列功能都需要打开sfsm_config.h中对应的配置开关才能支持。

### 支持函数状态

该功能使得每个拆分的函数都有状态，状态如下：

```c
typedef enum {
    SFSM_FUNC_STATE_NORMAL   = 0,   //!< 函数状态正常
    SFSM_FUNC_STATE_ABANDON,        //!< 函数不运行
    SFSM_FUNC_STATE_MAX
} sfsmFuncState;
```

一般情况下所有函数默认状态均为正常`SFSM_FUNC_STATE_NORMAL`，然后可以在运行过程中将函数状态设置为`SFSM_FUNC_STATE_ABANDON`，被设置为该状态的函数，后续就不会再运行到，直到状态机运行结束后，调用`sfsmStateReset`进行重置才会恢复。

要使用该功能，需要打开配置项：

```c
/**
  * @brief  每个函数都有状态支持 1：开启 0：关闭
  */
#define SFSM_FUNC_STATE_SUPPORT     1
```

变更函数状态的方式是通过返回值：

```c
SFSM_RETURN_STATE_ABANDON_FORWARD,   //!< 当前函数不再运行，接下来运行下一部分
SFSM_RETURN_STATE_ABANDON_JUMP,      //!< 当前函数不再运行，跳转到某一部分
```

如果是要使当前函数不再运行，接下来运行下一函数，就使用返回值`SFSM_RETURN_STATE_ABANDON_FORWARD`，例如：

```c
int taskFunction(sfsmFuncParm *parm)
{
    int ret = moduleFunction2();
    if(ret < 0)
        return SFSM_RETURN_STATE_ABANDON_FORWARD;   //下次运行下一个函数，然后该函数废弃，不再会被运行到
    else
        return SFSM_RETURN_STATE_FORWARD;   //下次运行下一个函数
}
```

如果是要使当前函数不再运行，接下来跳转到某一个函数，就使用返回值`SFSM_RETURN_STATE_ABANDON_JUMP`，例如：

```c
int taskFunction(sfsmFuncParm *parm)
{
    int ret = moduleFunction2();
    if(ret < 0) {
        int *ret = (int *)(parm->retparm);
        *ret = 1;                           //标记要跳转的函数ID
        return SFSM_RETURN_STATE_ABANDON_JUMP;      //下次运行到要跳转的函数，然后该函数废弃，不再会被运行到
    } else
        return SFSM_RETURN_STATE_FORWARD;   //下次运行下一个函数
}
```

### 支持Setup和Teardown函数

setup函数，在第一次运行状态机时运行，可以用于初始化或申请一些内容。

teardown函数，在状态机结束后自动触发，可以用于释放一些内容。

使用方式如下：

在配置文件中打开选项，如果只想单独支持Setup函数或者Teardown函数，可以只开启对应配置项。

```c
/**
  * @brief  支持Setup函数 1：开启 0：关闭
  */
#define SFSM_SETUP_FUNC_SUPPORT     1
/**
  * @brief  支持Teardown函数 1：开启 0：关闭
  */
#define SFSM_TEARDOWN_FUNC_SUPPORT  1
```

用户需要自定义Setup和Teardown函数，函数的类型为`void (*function)(void *parm)`，例如：

```c
//setup函数，在第一次运行状态机时运行，可以用于初始化或申请一些内容
void taskFunctionSetup(void *parm)
{
    /*...*/
}

//teardown函数，在状态机结束后自动触发，可以用于释放一些内容
void taskFunctionTeardown(void *parm)
{
    /*...*/
}
```

自定义的函数会将用户参数传递进来，如果有定义用户参数，可以按如下方式使用；如果没有定义用户参数，则为NULL。示例：

```c
void taskFunctionSetup(void *parm)
{
    userStruct *userData = (userStruct *)parm;
    userData->a = 0;
    userData->b = 0;
}
```

最后需要在状态机初始化完成后进行函数设置：

```c
//setup函数注册
sfsmSetSetupFunc(&moduleSfsm, taskFunctionSetup);
//teardown函数注册
sfsmSetTeardownFunc(&moduleSfsm, taskFunctionTeardown);
```

### 支持自动重载

正常使用的情况下，返回`SFSM_RETURN_STATE_END`了之后，状态机就不会再运行了，只有调用`sfsmStateReset`接口才能使状态机重置，重新运行。

有些场景下，我们只希望外部获取到状态机结束的状态，后续需要状态机再自动重新运行，这个时候可以打开自动重载的配置项。

```c
/**
  * @brief  支持自动重载 1：开启 0：关闭
  */
#define SFSM_AUTO_RELOAD            1
```

开启该配置后，在状态机运行结束后，我们可以获取到结束状态，并且下次调用`sfsmFuncRun`的时候状态机又会重新运行，状态自动恢复正常。

```c
    while(1) {
        sfsmFuncRun(&moduleSfsm);
        if(sfsmGetState(&moduleSfsm) == SFSM_STATE_END) {
            /*...*/
        }
    }
```

### 支持局部参数加载

函数中经常需要一些局部变量，但是对函数进行拆分后，局部变量就不方便使用了。

这个时候可以开启支持局部参数加载的功能了，这里提供两种局部参数，一种是生命周期维持在整个状态机的局部参数，一种是生命周期维持在单个函数的局部参数。两者的区别在于一个是状态机开始运行时申请，状态机运行结束后自动释放，另一个是单个函数运行开始时申请，离开跳转到其他函数时自动释放。

开启支持局部参数加载的配置项：

```c
/**
  * @brief  支持局部参数加载 1：开启 0：关闭
  */
#define SFSM_LOCAL_PARM             1
```

然后有几个选配的配置项：

```c
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
```

其中的`SFSM_MALLOC`和`SFSM_FREE`的配置项，如果支持malloc和free函数，则可以无需修改，但无OS的MCU很多是没有支持该功能的，需要自己实现malloc和free函数。

如果本身不支持malloc和free函数，可以使用本项目中自带的sfmemMalloc和sfmemFree函数，需要加入位于sfmem目录下的sfmem.c和sfmem.h文件。

```
sfsm
└── sfmem
    ├── sfmem.c
    └── sfmem.h
```

同时将配置项修改如下：

```c
#include "sfmem.h"

/**
  * @brief  局部参数内存申请，只有在 SFSM_LOCAL_PARM 开启时才有效
  */
#define SFSM_MALLOC                 sfmemMalloc

/**
  * @brief  局部参数内存释放，只有在 SFSM_LOCAL_PARM 开启时才有效
  */
#define SFSM_FREE                   sfmemFree
```

sfmem是预先通过静态数组占据一块内存大小作为内存池，然后申请释放该内存池的空间。

其中sfmem.h文件中的配置项就是用于设置内存池大小和内存块大小，默认内存池大小是128Byte，内存块为4Byte。可根据实际情况进行调整。

```c
/**
  * @brief  内存池大小
  */
#define SFMEM_POOL_SIZE         128
/**
  * @brief  内存块大小
  */
#define SFMEM_BLOCK_SIZE        4
```

回到局部参数加载的使用，在完成配置项配置后，需要在状态机初始化完成后的地方进行函数局部变量的大小设置，例如下面两个局部变量的大小均设置为16Byte：

```c
//设置局部变量的大小，生命周期维持在整个状态机的局部变量，生命周期维持在单个函数的局部变量
sfsmSetLocalParmSize(&moduleSfsm, 16, 16);
```

或者是按照所要使用的变量大小设置：

```c
//设置局部变量的大小，生命周期维持在整个状态机的局部变量，生命周期维持在单个函数的局部变量
sfsmSetLocalParmSize(&moduleSfsm, sizeof(userStruct), sizeof(userStruct));
```

如果其中一种局部变量不需要使用，则可以设置为0。

也可以在配置项中设置默认分配的内存大小，这样就可以不用调用sfsmSetLocalParmSize了。如下：

```c
/**
  * @brief  局部参数默认分配内存大小，生命周期维持在整个状态机，只有在 SFSM_LOCAL_PARM 开启时才有效
  */
#define SFSM_LOCAL_PARM_SIZE        16

/**
  * @brief  局部参数默认分配内存大小，生命周期维持在单个函数，只有在 SFSM_LOCAL_PARM 开启时才有效
  */
#define SFSM_PART_LOCAL_PARM_SIZE   16
```

但是通过调用sfsmSetLocalParmSize来设置会更加的灵活，如果有多个状态机的话可以设置不同的大小。

完成设置后，接下来看下如何使用。

假如函数如下，taskFunction1函数运行结束后会运行taskFunction2。最开始运行时，申请的局部函数空间数值都会被置为0，所以所有参数打印结果都为0，但当运行到taskFunction2函数时，partLocalParm的数值都被清空为0，而localParm的数值还保留。

```c
int taskFunction1(sfsmFuncParm *parm)
{
    userStruct *localParm = (userStruct *)(parm->localParm);
    printf("localParm1 %d %d\n", localParm->a++, localParm->b++);

    userStruct *partLocalParm = (userStruct *)(parm->partLocalParm);
    printf("partLocalParm1 %d %d\n", partLocalParm->a++, partLocalParm->b++);

    return SFSM_RETURN_STATE_FORWARD;   //下次运行下一个函数
}

int taskFunction2(sfsmFuncParm *parm)
{
    userStruct *localParm = (userStruct *)(parm->localParm);
    printf("localParm2 %d %d\n", localParm->a++, localParm->b++);

    userStruct *partLocalParm = (userStruct *)(parm->partLocalParm);
    printf("partLocalParm2 %d %d\n", partLocalParm->a++, partLocalParm->b++);

    return SFSM_RETURN_STATE_FORWARD;   //下次运行下一个函数
}
```

假如函数如下，taskFunction函数会根据返回值来判断是保持循环还是运行下一个函数。其中partLocalParm的数值在一开始为0，之后循环中会一直增加，直到离开这个函数后，才会自动清零。

```c
int taskFunction(sfsmFuncParm *parm)
{
    userStruct *partLocalParm = (userStruct *)(parm->partLocalParm);

    int ret = moduleFunction2();
    if(ret < 0) {
        printf("partLocalParm %d %d\n", partLocalParm->a++, partLocalParm->b++);
        return SFSM_RETURN_STATE_HOLD;   //下次运行当前函数
    } else
        return SFSM_RETURN_STATE_FORWARD;   //下次运行下一个函数
}
```

### 支持函数标记

函数标记用于相同的函数需要传递一些默认参数，或者需要可以通过不同的标记来实现不同功能。

要使用该功能，需要打开配置项：

```c
/**
  * @brief  每个函数都有默认标记 1：开启 0：关闭
  */
#define SFSM_DEFAULT_FLAG_PARM      1
```

然后在定义状态表时将标记填入，如果不需要标记值则置位NULL。例如：

```c
sfsmStateTable table[] =
{
    //拆分的函数，      跳转ID  标记值
    { taskFunction1 ,     0, NULL},
    { taskFunction2 ,     0, (void *)1},
    { taskFunction2 ,     0, (void *)2},
    { taskFunction3 ,     0, NULL },
};
```

然后在函数中使用标记的数值。

```c
int taskFunction2(sfsmFuncParm *parm)
{
    size_t flag = (size_t)(parm->flagParm);//获取标记
    moduleFunction2(flag);
    return SFSM_RETURN_STATE_FORWARD;   //下次运行下一个函数
}
```

### 支持延迟调用

延迟调用适用于有些函数需要延时功能。

要使用该功能，需要打开配置项：

```c
/**
  * @brief  支持延迟调用功能 1：开启 0：关闭
  */
#define SFSM_DELAY                  1
```

用户需要自定义获取时间的函数，函数的类型为`unsigned int  (*function)(void)`。例如，假设在 1 ms 的定时器中有一个计数值每一毫秒自增一下，然后自定义一个获取时间的函数`getTimeFunc`用于返回计数值：

```c
void timer1ms(void) {
    cnt++;
}

unsigned int getTimeFunc(void) {
    return cnt;
}
```

然后需要在状态机初始化完成后进行函数设置，`getTimeFunc` 就是传入上一步自定义获取时间的函数，`timeMaxCnt`则是时间的最大值。

```c
void sfsmSetDelayParm(sfsmType* pSfsm, unsigned int (*getTimeFunc)(void), unsigned int timeMaxCnt);
```

例如获取时间的函数为`getTimeFunc`，假设计数值为16位数，最大值就是65535。

```c
//设置延时相关函数
sfsmSetDelayParm(&moduleSfsm, getTimeFunc, 65535);
```

也可以通过配置项配置默认的获取时间函数与时间的最大值：

```c
/**
  * @brief  设置默认的获取时间函数，只有在 SFSM_DELAY 开启时才有效
  */
#define SFSM_GET_TIME_FUNC          NULL

/**
  * @brief  设置默认的时间最大值，只有在 SFSM_DELAY 开启时才有效
  */
#define SFSM_TIME_MAX_CNT           0
```

例如配置为：

```c
/**
  * @brief  设置默认的获取时间函数，只有在 SFSM_DELAY 开启时才有效
  */
#define SFSM_GET_TIME_FUNC          getTimeFunc

/**
  * @brief  设置默认的时间最大值，只有在 SFSM_DELAY 开启时才有效
  */
#define SFSM_TIME_MAX_CNT           65535
```

使用配置项配置的话，就可以不需要调用`sfsmSetDelayParm`进行设置，默认采用配置项的参数。配置项适用于所有的状态机都使用相同的获取时间函数的情况，如果有个别的状态机需要使用不同的获取时间函数，可以单独调用`sfsmSetDelayParm`进行设置。

完成配置之后，就可以正常使用了。

一共提供两种返回值，如下：

```c
SFSM_RETURN_STATE_DELAY,            //!< 延后一段时间后再运行
SFSM_RETURN_STATE_DELAY_FORWARD,    //!< 延后一段时间后，接下来运行下一部分
```

例如，函数`taskFunction1`运行完成后，要延迟 10 ms，先设置延时的时间，然后设置返回值：

```c
int taskFunction1(sfsmFuncParm *parm)
{
    printf("moduleFunction1\n");
    int *ret = (int *)(parm->retparm);
    *ret = 10;                              //设置延时时间10ms
    return SFSM_RETURN_STATE_DELAY_FORWARD; //延后一段时间后，接下来运行下一部分
}
```

### 多配置同时开启

支持任意配置的组合搭配开启，但是要注意，多配置同时开启时，定义状态表要注意参数的位置，需要按照状态表的参数顺序来填写。

```c
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
```

### 多级嵌套

支持多个状态机的多级嵌套，例如：

```c
int taskFunction2(sfsmFuncParm *parm)
{
    sfsmFuncRun(&module2Sfsm);
    if(sfsmGetState(&module2Sfsm) != SFSM_STATE_END) {
        return SFSM_RETURN_STATE_HOLD;
    }

    /*...*/

    return SFSM_RETURN_STATE_FORWARD;   //下次运行下一个函数
}
```

嵌套的状态机module2Sfsm如果没运行完就返回`SFSM_RETURN_STATE_HOLD`，这样下次就会继续运行该状态机，运行结束后就继续前进。

## License

MIT License

Copyright (c) 2022 Hong Jiahua

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
