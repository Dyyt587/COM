/***
	*************************************************************************************************
	*	@file  	main.c
	*	@version V1.0
	*  @date    2021-8-4
	*	@author  反客科技	
	*	@brief   LED闪烁实验
   *************************************************************************************************
   *  @description
	*
	*	实验平台：反客STM32H7B0VBT6核心板 （型号：FK7B0M1-VBT6）
	*	淘宝地址：https://shop212360197.taobao.com
	*	QQ交流群：536665479
	*
>>>>> 文件说明：
	*
	*	初始化LED的IO口，配置为推挽输出、速度等级2M。
	*
	************************************************************************************************
***/

#include "stm32h7xx_hal.h"
#include "led.h"  


void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_LED1_CLK_ENABLE;		// 初始化LED1 GPIO时钟	


	HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);		// LED1引脚输出低，即点亮LED1


	GPIO_InitStruct.Pin 		= LED1_PIN;					// LED1引脚
	GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;	// 推挽输出模式
	GPIO_InitStruct.Pull 	= GPIO_NOPULL;				// 不上下拉
	GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_LOW;	// 低速模式
	HAL_GPIO_Init(LED1_PORT, &GPIO_InitStruct);


}

