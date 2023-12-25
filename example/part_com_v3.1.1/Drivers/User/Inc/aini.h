#ifndef __AINI_H
#define __AINI_H
/********************************************************************
*@beief 自动化初始化模块,依赖编译器,目前仅支持 CC_ARM 环境
*灵感来自于 RT-Thread 的自动初始化思路,对此表示感谢
*使用此模块可以让代码变的简洁
*
*@autor james.chen
*@email bai_yechuang@163.com
**********************************************************************/



//#include "elog.h"
#define AINI_LOG(x,...)// log_e(x ,##__VA_ARGS__)
#define AINI_USE_PREMAIN     1





#if defined(__CC_ARM)|| defined(__CLANG_ARM) || (__ARMCC_VERSION >= 6000000) || (defined ( __GNUC__ ) && !defined (__CC_ARM))//ac6 ac5 gnu

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) /* ARM Compiler V6 */
	#define AINI_USED        __attribute__((used))
	#define AINI_SECTION(x)  __attribute__((section(x)))
#elif defined ( __GNUC__ ) && !defined (__CC_ARM) /* GNU Compiler */
	#define AINI_USED        __attribute__((used))
	#define AINI_SECTION(x)  __attribute__((section(x)))
#elif (defined(__CC_ARM)|| defined(__CLANG_ARM))  /* ARM Compiler V5 */
    // 这里是针对 ARM Compiler 5 (AC5) 的代码
#define AINI_USED        __attribute__((used))
#define AINI_SECTION(x)  __attribute__((section(x)))
#endif /* __GNUC__ */

/**
 * @brief 申明函数指针  os_init_fn_t 
 * 形参:void
 * 返回:int
 * 扩展:函数指针是指向函数的指针变量,它本身是指针变量,指向函数地址
 */
typedef int (*aini_init_fn_t)(void);

#define AINI_INIT_EXPORT(fn,level) \
    AINI_USED const aini_init_fn_t __aini_call_##fn AINI_SECTION(".aini_call." level) = fn

//板级初始化 顺序1
#define AINI_BOARD_INIT(fn)      AINI_INIT_EXPORT(fn,"1")
    
//设备初始化 顺序3
#define AINI_DEVICE_INIT(fn)     AINI_INIT_EXPORT(fn,"2")
    
//组件初始化 顺序4
#define AINI_COMPONENT_INIT(fn)  AINI_INIT_EXPORT(fn,"3")
    
//环境初始化 顺序5
#define AINI_ENV_INIT(fn)        AINI_INIT_EXPORT(fn,"4")
    
//APP初始化 顺序6
#define AINI_APP_INIT(fn)        AINI_INIT_EXPORT(fn,"5")

/**
*这个函数需要重写
*主要执行系统启动后最早期的必须按照顺序执行的函数
*比如 先执行 复位外设,配置系统时钟,初始化IO,初始化打印串口等
*/    
extern int ani_seq_init(void);
    /* 3.初始化板级设备 */
extern void   aini_components_board_init(void);
    
    /* 4.初始化一些软件级别的初始化，如库,APP等 */
extern void   aini_components_init(void);
#else
#error 'not support complier!'
#endif


#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) /* ARM Compiler V6 */
  #ifndef __weak
    #define __weak  __attribute__((weak))
  #endif
  #ifndef __packed
    #define __packed  __attribute__((packed))
  #endif
#elif defined ( __GNUC__ ) && !defined (__CC_ARM) /* GNU Compiler */
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
  #ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif /* __packed */
#endif /* __GNUC__ */



#endif

