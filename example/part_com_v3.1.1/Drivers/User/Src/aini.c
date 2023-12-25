#include "aini.h"

static int aini_start(void)
{
//    serial_hw_console_output("os_start\r\n");
    return 0;
}
AINI_INIT_EXPORT(aini_start,"0");

static int aini_board_start(void)
{
//    serial_hw_console_output("os_board_start\r\n");
    return 0;
}
AINI_INIT_EXPORT(aini_board_start,"0.end");

static int aini_board_end(void)
{
//    serial_hw_console_output("os_board_end\r\n");
    return 0;
}
AINI_INIT_EXPORT(aini_board_end,"1.end");

static int aini_end(void)
{
//    serial_hw_console_output("os_end\r\n");
    return 0;
}
AINI_INIT_EXPORT(aini_end,"5.end");


/**
 * @brief �Զ����� OS_BOARD_INIT(fn) �Զ��ĺ���
 * .init_call.1
 */
 void aini_components_board_init(void)
{
    volatile const aini_init_fn_t *pfn;
    for(pfn = &__aini_call_aini_board_start; pfn < &__aini_call_aini_board_end;pfn++)
    {
        int rec = (*pfn)();
        if(rec!=0){
            AINI_LOG("The func addr at %x init code is not zero but %d\r\n",(unsigned int)pfn,rec);
        }
    }

}

/**
 * @brief �Զ���ʼ�� section .init_call.2 ~ .init_call.5
 * 
 */
 void aini_components_init(void)
{
    volatile const aini_init_fn_t *pfn;
	int rec=0;
    for(pfn = &__aini_call_aini_board_end; pfn < &__aini_call_aini_end; pfn++)
    {
        rec = (*pfn)();
        if(rec!=0){
            AINI_LOG("The func addr at %x init code is not zero but %d\r\n",(unsigned int)pfn,rec);
        }
    }
}


#if ( defined(__CC_ARM)|| defined(__CLANG_ARM) || (__ARMCC_VERSION >= 6000000) || (defined ( __GNUC__ ) && !defined (__CC_ARM))) && (AINI_USE_PREMAIN !=0) //ac6 ac5
extern int $Super$$main(void);
/** 
*$Sub$$main �ض���main����,��mian����֮ǰִ��
*/
int $Sub$$main(void)
{
    /* 1.���ж� */
    //do nothing
    
    /* 2.��ʼ��ϵͳʱ��,Ƭ����Դ */
    ani_seq_init();
    
    /* 3.��ʼ���弶�豸 */
    aini_components_board_init();
    
    /* 4.��ʼ��һЩ�������ĳ�ʼ�������,APP�� */
    aini_components_init();
    
    /* 5. ���ж� */
    //do nothing
    
    //#if defined(__CC_ARM) || defined(__CLANG_ARM)
		#if (defined(__CC_ARM)|| defined(__CLANG_ARM) || (__ARMCC_VERSION >= 6000000)) && (AINI_USE_PREMAIN !=0) //ac6 ac5
    extern int main(void);
    extern int $Super$$main(void);
    /* 6.��ת��main���� */
    $Super$$main(); /* for ARMCC. */
    #else
    #error 'not support complier!'
    #endif
    
    return 0;
}
#elif (AINI_USE_PREMAIN == 0)
#else
#error 'not support complier!'
#endif

__weak int ani_seq_init(void)
{
   /* ��ʼ��ϵͳʱ��,Ƭ����Դ */
	return 0;
}
/**
 * @brief gcc ����
 * gcc -o main example_section.c -Wl,-Map,example_section.map
 * 
 */

