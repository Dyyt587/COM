#ifndef __AINI_H
#define __AINI_H
/********************************************************************
*@beief �Զ�����ʼ��ģ��,����������,Ŀǰ��֧�� CC_ARM ����
*��������� RT-Thread ���Զ���ʼ��˼·,�Դ˱�ʾ��л
*ʹ�ô�ģ������ô����ļ��
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
    // ��������� ARM Compiler 5 (AC5) �Ĵ���
#define AINI_USED        __attribute__((used))
#define AINI_SECTION(x)  __attribute__((section(x)))
#endif /* __GNUC__ */

/**
 * @brief ��������ָ��  os_init_fn_t 
 * �β�:void
 * ����:int
 * ��չ:����ָ����ָ������ָ�����,��������ָ�����,ָ������ַ
 */
typedef int (*aini_init_fn_t)(void);

#define AINI_INIT_EXPORT(fn,level) \
    AINI_USED const aini_init_fn_t __aini_call_##fn AINI_SECTION(".aini_call." level) = fn

//�弶��ʼ�� ˳��1
#define AINI_BOARD_INIT(fn)      AINI_INIT_EXPORT(fn,"1")
    
//�豸��ʼ�� ˳��3
#define AINI_DEVICE_INIT(fn)     AINI_INIT_EXPORT(fn,"2")
    
//�����ʼ�� ˳��4
#define AINI_COMPONENT_INIT(fn)  AINI_INIT_EXPORT(fn,"3")
    
//������ʼ�� ˳��5
#define AINI_ENV_INIT(fn)        AINI_INIT_EXPORT(fn,"4")
    
//APP��ʼ�� ˳��6
#define AINI_APP_INIT(fn)        AINI_INIT_EXPORT(fn,"5")

/**
*���������Ҫ��д
*��Ҫִ��ϵͳ�����������ڵı��밴��˳��ִ�еĺ���
*���� ��ִ�� ��λ����,����ϵͳʱ��,��ʼ��IO,��ʼ����ӡ���ڵ�
*/    
extern int ani_seq_init(void);
    /* 3.��ʼ���弶�豸 */
extern void   aini_components_board_init(void);
    
    /* 4.��ʼ��һЩ�������ĳ�ʼ�������,APP�� */
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

