#include "unity.h"
#include "stdio.h"



#if 0
#pragma import(__use_no_semihosting)
 
/* 定义 _sys_exit() 以避免使用半主机模式 */
 
void _sys_exit(int x)
 
{
 
    x = x;
 
}
 
/* 标准库需要的支持类型 */
 
struct __FILE
 
{
 
    int handle;
 
};
 
FILE __stdout;

//__use_no_semihosting was requested, but _ttywrch was 
void _ttywrch(int ch)
{
    ch = ch;
}

/*************************************************************************************************
*	函 数 名:	fputc
*	入口参数:	ch - 要输出的字符 ，  f - 文件指针（这里用不到）
*	返 回 值:	正常时返回字符，出错时返回 EOF（-1）
*	函数功能:	重定向 fputc 函数，目的是使用 printf 函数
*	说    明:	无		
*************************************************************************************************/

int fputc(int ch, FILE *f)
{	    

		   // com_tx(&com,(uint8_t*)(&ch),1);

	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 100);	// 发送单字节数据
	//com_comunite(&com_test,tx, (uint8_t *)&ch,1);
	return (ch);
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
#endif












