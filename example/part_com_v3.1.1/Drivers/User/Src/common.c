#include "common.h"
#include "usart.h"
#include "stm32h7xx_hal.h"
#include "stdio.h"
//#include "com_component.h"



 
/* ------------------通过重定向将printf函数映射到串口1上-------------------*/
#if !defined(__MICROLIB)

//#pragma import(__use_no_semihosting)
__asm (".global __use_no_semihosting\n\t");
void _sys_exit(int x) //避免使用半主机模式
{
  x = x;
}
//__use_no_semihosting was requested, but _ttywrch was 
void _ttywrch(int ch)
{
    ch = ch;
}
struct __FILE
{
  int handle;
};
FILE __stdout;

#endif

#if defined ( __GNUC__ ) && !defined (__clang__) 
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif 
PUTCHAR_PROTOTYPE
{
  /* 实现串口发送一个字节数据的函数 */
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);
  return ch;
}


/*************************************************************************************************
*	函 数 名:	printf_test
*	入口参数:	无
*	返 回 值:	无
*	函数功能:printf及调试串口的通讯测试
*	说    明:	无		
*************************************************************************************************/
void printf_test(void)
{

  printf("STM32H7B0 串口测试 \r\n");

}













