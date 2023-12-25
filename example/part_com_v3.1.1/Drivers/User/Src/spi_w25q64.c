/***
	************************************************************************************************************************************************************************************************
	*	@file   	spi_w25q64.c
	*	@version V1.0
	*  @date    2021-8-4
	*	@author  反客科技
	*	@brief   SPI 驱动 W25Qxx系列相关函数
   **********************************************************************************************************************************************************************************************
   *  @description
	*
	*	实验平台：反客STM32H7B0VBT6核心板 （型号：FK7B0M1-VBT6），使用的是W25Q64
	*	淘宝地址：https://shop212360197.taobao.com
	*	QQ交流群：536665479
	*
>>>>> 文件说明：
	*
	*  1. 使用硬件 SPI6 驱动 W25Q64
	*  2. 提供的读写函数均使用HAL库函数直接操作，没有用到DMA和中断
   *  3. SPI6 SCK驱动时钟配置为 50MHz
   *
>>>>> 重要说明：
	*
	*	1. W25Q64JV的擦除时间是限定的!!! 手册给出的典型参考值为: 4K-45ms, 32K-120ms ,64K-150ms,整片擦除20S
	*
	*	2. W25Q64JV的写入时间是限定的!!! 手册给出的典型参考值为: 256字节-0.4ms，也就是 1M字节/s （实测大概在600K字节/s左右）
	*
	*  3. W25Q64JV所允许的最高驱动频率虽然不止50MHz，但使用读取指令 0x03 所允许的最大速度只能到50M
	* 
	*	4. 实际使用中，当数据比较大时，建议使用64K或者32K擦除，擦除时间比4K擦除块	
	*
	*********************************************************************************************************************************************************************************************FANKE*****
***/

#include "spi_w25q64.h"
#include "stdio.h"
#include "spi.h"

//SPI_HandleTypeDef hspi6;	// SPI_HandleTypeDef 结构体变量

/*************************************************************************************************
*	函 数 名:	HAL_SPI_MspInit
*	入口参数:	hspi - SPI_HandleTypeDef定义的变量，即表示定义的 SPI 句柄
*	返 回 值:	无
*	函数功能:	初始化 SPI6 引脚
*	说    明:	无		
*************************************************************************************************/

/*************************************************************************************************
*	函 数 名:	MX_SPI6_Init
*	入口参数:	无
*	返 回 值:	无
*	函数功能:	初始化SPI配置
*	说    明:	使用硬件片选	 
*************************************************************************************************/

 
/*************************************************************************************************
*	函 数 名: SPI_W25Qxx_Init
*	入口参数: 无
*	返 回 值: SPI_W25Qxx_OK - 初始化成功，W25Qxx_ERROR_INIT - 初始化错误
*	函数功能: 初始化 SPI 配置，读取W25Q64ID
*	说    明: 无	
*************************************************************************************************/

int8_t	SPI_W25Qxx_Init(void)
{
 	uint32_t	Device_ID;	// 定义变量存储器件ID

	//MX_SPI6_Init();  							 		// 初始化 SPI6 配置
   Device_ID =  SPI_W25Qxx_ReadID();			// 读取ID

	if( Device_ID == SPI_W25Qxx_FLASH_ID )		// 进行匹配
	{
		printf ("W25Q64 OK,flash ID:%X\r\n",Device_ID);		// 初始化成功
		return SPI_W25Qxx_OK;			// 返回成功标志		
	}
	else
	{
		printf ("W25Q64 ERROR!!!!!  ID:%X\r\n",Device_ID);	// 初始化失败	
		return W25Qxx_ERROR_INIT;		// 返回错误标志
	}	
}

/*************************************************************************************************
*	函 数 名: SPI_W25Qxx_ReadID
*	入口参数: 无
*	返 回 值: W25Qxx_ID - 读取到的器件ID，W25Qxx_ERROR_INIT - 通信、初始化错误
*	函数功能: 读取器件ID
*	说    明: 无	
**************************************************************************************************/

uint32_t SPI_W25Qxx_ReadID(void)
{
   uint8_t	SPI_TX_Buff[1] __attribute__ ((at(0x38000000)));      // 要发送的指令
	uint8_t	SPI_RX_Buff[4] __attribute__ ((at(0X38000100)));		// 存储SPI读到的数据
   uint32_t W25Qxx_ID;           // ID
	SPI_TX_Buff[0] = SPI_W25Qxx_CMD_JedecID;		// 发送读JedecID命令 
		HAL_GPIO_WritePin(W25Q_NSS_GPIO_Port,W25Q_NSS_Pin,GPIO_PIN_RESET);

	if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)SPI_RX_Buff, 1, 1000) != HAL_OK)	
	{
		return ERROR;
	}	
	if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)&SPI_RX_Buff[1], 3, 1000) != HAL_OK)	
	{
		return ERROR;
	}
		HAL_GPIO_WritePin(W25Q_NSS_GPIO_Port,W25Q_NSS_Pin,GPIO_PIN_SET);

// 需要注意的，使用 HAL_SPI_TransmitReceive() 进行通信，发送第一个读ID命令时，就会立即读取一个字节
// 因此，要想得到实际的数据需要偏移一个字节
   W25Qxx_ID = (SPI_RX_Buff[1] << 16) | (SPI_RX_Buff[2] << 8) | SPI_RX_Buff[3];    // 将读到的数据组合为ID

   return W25Qxx_ID; // 返回ID值
}

/*************************************************************************************************
*	函 数 名: SPI_W25Qxx_WaitForWriteEnd  
*	入口参数: 无
*	返 回 值: SPI_W25Qxx_OK - 通信正常结束，W25Qxx_ERROR_Wait - 等待错误
*	函数功能: 等待W25Qxx操做完毕
*	说    明: 擦除/写入数据时，需要调用该函数等待操做完成	
**************************************************************************************************/

int8_t SPI_W25Qxx_WaitForWriteEnd(void)
{
   uint8_t	SPI_TX_Buff[1];   // 要发送的指令
	uint8_t	SPI_RX_Buff[2];	// 存储SPI读到的数据 

   while (1)
   {
	   SPI_TX_Buff[0] = SPI_W25Qxx_CMD_ReadStatus_REG1;		// 发送读 读状态寄存器 命令 
      SPI_RX_Buff[1] = 0;     // 将存储区置零

      if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)SPI_RX_Buff, 2, 1000) != HAL_OK)	
      {
         return W25Qxx_ERROR_Wait;
      }	  

// 读状态寄存器1的第0位（只读），Busy标志位，当正在擦除/写入数据/写命令时会被置1
      if( (SPI_RX_Buff[1] & SPI_W25Qxx_CMD_Status_REG1_BUSY)  != SET )  // 比较标志位
      {
         return SPI_W25Qxx_OK;
      }   
   }
}

/*************************************************************************************************
*	函 数 名: SPI_W25Qxx_WriteEnable
*	入口参数: 无
*	返 回 值: QSPI_W25Qxx_OK - 写使能成功，W25Qxx_ERROR_WriteEnable - 写使能失败
*	函数功能: 发送写使能命令
*	说    明: 在擦除或者写入数据之前，需要调用该函数进行写使能
**************************************************************************************************/

int8_t SPI_W25Qxx_WriteEnable(void)
{
   uint8_t	SPI_TX_Buff[1];   // 要发送的指令
	uint8_t	SPI_RX_Buff[1];	// 存储SPI读到的数据，此处用不到 

	SPI_TX_Buff[0] = SPI_W25Qxx_CMD_WriteEnable;		// 发送读 写使能 命令 

	if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)SPI_RX_Buff, 1, 1000) != HAL_OK)	
	{
		return W25Qxx_ERROR_WriteEnable;
	}	
   return SPI_W25Qxx_OK;
}

/*************************************************************************************************
*
*	函 数 名: SPI_W25Qxx_SectorErase
*
*	入口参数: SectorAddress - 要擦除的地址
*
*	返 回 值: SPI_W25Qxx_OK - 擦除成功
*			    W25Qxx_ERROR_Erase - 擦除失败
*
*	函数功能: 进行扇区擦除操作，每次擦除4K字节
*
*	说    明: 1.按照 W25Q64JV 数据手册给出的擦除参考时间，典型值为 45ms，最大值为400ms
*				 2.实际的擦除速度可能大于45ms，也可能小于45ms
*				 3.flash使用的时间越长，擦除所需时间也会越长
*
**************************************************************************************************/

int8_t SPI_W25Qxx_SectorErase(uint32_t SectorAddress)	
{
   uint8_t	SPI_TX_Buff[4];   // 要发送的指令
	uint8_t	SPI_RX_Buff[1];	// 存储SPI读到的数据，此处用不到 

   SPI_W25Qxx_WriteEnable();  // 写使能

   SPI_TX_Buff[0] = SPI_W25Qxx_CMD_SectorErase;          // 4K扇区擦除指令
   SPI_TX_Buff[1] = ( SectorAddress & 0xFF0000 ) >> 16;  // 发送要擦除的地址
   SPI_TX_Buff[2] = ( SectorAddress & 0xFF00 ) >> 8;;
   SPI_TX_Buff[3] =   SectorAddress & 0xFF;

	if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)SPI_RX_Buff, 4, 1000) != HAL_OK)	
	{
		return W25Qxx_ERROR_Erase;
	}	
   
   SPI_W25Qxx_WaitForWriteEnd(); // 等待擦除结束

	return SPI_W25Qxx_OK; // 擦除成功
}

/*************************************************************************************************
*
*	函 数 名: SPI_W25Qxx_BlockErase_32K
*
*	入口参数: SectorAddress - 要擦除的地址
*
*	返 回 值: SPI_W25Qxx_OK - 擦除成功
*			    W25Qxx_ERROR_Erase - 擦除失败
*
*	函数功能: 进行块擦除操作，每次擦除32K字节
*
*	说    明: 1.按照 W25Q64JV 数据手册给出的擦除参考时间，典型值为 120ms，最大值为1600ms
*				 2.实际的擦除速度可能大于120ms，也可能小于120ms
*				 3.flash使用的时间越长，擦除所需时间也会越长
*
*************************************************************************************************/

int8_t SPI_W25Qxx_BlockErase_32K(uint32_t SectorAddress)	
{
   uint8_t	SPI_TX_Buff[4];   // 要发送的指令
	uint8_t	SPI_RX_Buff[1];	// 存储SPI读到的数据，此处用不到 

   SPI_W25Qxx_WriteEnable();  // 写使能

   SPI_TX_Buff[0] = SPI_W25Qxx_CMD_BlockErase_32K;       // 32K块擦除指令
   SPI_TX_Buff[1] = ( SectorAddress & 0xFF0000 ) >> 16;  // 发送要擦除的地址
   SPI_TX_Buff[2] = ( SectorAddress & 0xFF00 ) >> 8;;
   SPI_TX_Buff[3] =   SectorAddress & 0xFF;

	if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)SPI_RX_Buff, 4, 1000) != HAL_OK)	
	{
		return W25Qxx_ERROR_Erase;
	}	
   
   SPI_W25Qxx_WaitForWriteEnd(); // 等待擦除完毕

	return SPI_W25Qxx_OK; // 擦除成功
}

/*************************************************************************************************
*
*	函 数 名: SPI_W25Qxx_BlockErase_64K
*
*	入口参数: SectorAddress - 要擦除的地址
*
*	返 回 值: SPI_W25Qxx_OK - 擦除成功
*			    W25Qxx_ERROR_Erase - 擦除失败
*
*	函数功能: 进行块擦除操作，每次擦除64K字节
*
*	说    明: 1.按照 W25Q64JV 数据手册给出的擦除参考时间，典型值为 150ms，最大值为2000ms
*				 2.实际的擦除速度可能大于150ms，也可能小于150ms
*				 3.flash使用的时间越长，擦除所需时间也会越长
*				 4.实际使用建议使用64K擦除，擦除的时间最快
*
**************************************************************************************************/

int8_t SPI_W25Qxx_BlockErase_64K(uint32_t SectorAddress)	
{
   uint8_t	SPI_TX_Buff[4];   // 要发送的指令
	uint8_t	SPI_RX_Buff[1];	// 存储SPI读到的数据，此处用不到 

   SPI_W25Qxx_WriteEnable();  // 写使能   

   SPI_TX_Buff[0] = SPI_W25Qxx_CMD_BlockErase_64K;       // 64K块擦除指令
   SPI_TX_Buff[1] = ( SectorAddress & 0xFF0000 ) >> 16;  // 发送要擦除的地址
   SPI_TX_Buff[2] = ( SectorAddress & 0xFF00 ) >> 8;;
   SPI_TX_Buff[3] =   SectorAddress & 0xFF;

	if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)SPI_RX_Buff, 4, 1000) != HAL_OK)	
	{
		return W25Qxx_ERROR_Erase;
	}	
   
   SPI_W25Qxx_WaitForWriteEnd();    // 等待擦除结束

	return SPI_W25Qxx_OK; // 擦除成功
}

/*************************************************************************************************
*
*	函 数 名: SPI_W25Qxx_ChipErase
*
*	入口参数: 无
*
*	返 回 值: SPI_W25Qxx_OK - 擦除成功
*			    W25Qxx_ERROR_Erase - 擦除失败
*
*	函数功能: 进行整片擦除操作
*
*	说    明: 1.按照 W25Q64JV 数据手册给出的擦除参考时间，典型值为 20s，最大值为100s
*				 2.实际的擦除速度可能大于20s，也可能小于20s
*				 3.flash使用的时间越长，擦除所需时间也会越长
*
*************************************************************************************************/

int8_t SPI_W25Qxx_ChipErase(void)	
{
   uint8_t	SPI_TX_Buff[1];   // 要发送的指令
	uint8_t	SPI_RX_Buff[1];	// 存储SPI读到的数据，此处用不到 

   SPI_W25Qxx_WriteEnable();  // 写使能

   SPI_TX_Buff[0] = SPI_W25Qxx_CMD_ChipErase;   // 全片擦除指令

	if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)SPI_RX_Buff, 1, 1000) != HAL_OK)	
	{
		return W25Qxx_ERROR_Erase;
	}	
   
   SPI_W25Qxx_WaitForWriteEnd();

	return SPI_W25Qxx_OK; // 擦除成功
}

/**********************************************************************************************************
*
*	函 数 名: SPI_W25Qxx_WritePage
*
*	入口参数: pBuffer 		 - 要写入的数据
*				 WriteAddr 		 - 要写入 W25Qxx 的地址
*				 NumByteToWrite - 数据长度，最大只能256字节
*
*	返 回 值: SPI_W25Qxx_OK 		     - 写数据成功
*				 W25Qxx_ERROR_TRANSMIT	  - 传输失败
*
*	函数功能: 按页写入，最大只能256字节，在数据写入之前，请务必完成擦除操作
*
*	说    明: 1.Flash的写入时间是限定的
*				 2.按照 W25Q64JV 数据手册给出的 页(256字节) 写入参考时间，典型值为 0.4ms，最大值为3ms
*				 3.实际的写入速度可能大于0.4ms，也可能小于0.4ms
*				 4.Flash使用的时间越长，写入所需时间也会越长
*				 5.在数据写入之前，请务必完成擦除操作
*
***********************************************************************************************************/

int8_t SPI_W25Qxx_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t	SPI_TX_Buff[260];    // 要发送的指令	
	uint16_t i = 0;               // 计数变量

   SPI_W25Qxx_WriteEnable();     // 写使能

   SPI_TX_Buff[0] = W25Qxx_CMD_PageProgram;           // 页编程指令
   SPI_TX_Buff[1] = ( WriteAddr & 0xFF0000 ) >> 16;   // 地址
   SPI_TX_Buff[2] = ( WriteAddr & 0xFF00 ) >> 8;;
   SPI_TX_Buff[3] =   WriteAddr & 0xFF;

   for ( i = 0; i < NumByteToWrite; i++)  // 复制要写入的数据
   {
      SPI_TX_Buff[4+i] = * pBuffer;
      pBuffer++;
   }

	if( HAL_SPI_Transmit(&hspi6, (uint8_t *)SPI_TX_Buff, NumByteToWrite+4, 1000)!= HAL_OK )	// 开始传输
	{
		return W25Qxx_ERROR_TRANSMIT;
	}	
   
   SPI_W25Qxx_WaitForWriteEnd(); // 等待写入完成

	return SPI_W25Qxx_OK; // 写入成功
}

/**********************************************************************************************************
*
*	函 数 名: SPI_W25Qxx_WriteBuffer
*
*	入口参数: pBuffer 		 - 要写入的数据
*				 WriteAddr 		 - 要写入 W25Qxx 的地址
*				 NumByteToWrite - 数据长度，最大不能超过flash芯片的大小
*
*	返 回 值: QSPI_W25Qxx_OK 		     - 写数据成功
*			    W25Qxx_ERROR_WriteEnable - 写使能失败
*				 W25Qxx_ERROR_TRANSMIT	  - 传输失败
*
*	函数功能: 写入数据，最大不能超过flash芯片的大小，写数据前请务必完成擦除操作
*
*	说    明: 1.Flash的写入时间是有限定的
*				 2.按照 W25Q64JV 数据手册给出的 页 写入参考时间，典型值为 0.4ms，最大值为3ms
*				 3.实际的写入速度可能大于0.4ms，也可能小于0.4ms
*				 4.Flash使用的时间越长，写入所需时间也会越长
*				 5.在数据写入之前，请务必完成擦除操作
*				 6.该函数移植于 stm32h743i_eval_qspi.c
*
**********************************************************************************************************/

int8_t SPI_W25Qxx_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t Size)
{	
	uint32_t end_addr=0, current_size=0, current_addr=0;
	uint8_t *write_data;  // 要写入的数据

	current_size = SPI_W25Qxx_PageSize - (WriteAddr % SPI_W25Qxx_PageSize); // 计算当前页还剩余的空间

	if (current_size > Size)	// 判断当前页剩余的空间是否足够写入所有数据
	{
		current_size = Size;		// 如果足够，则直接获取当前长度
	}

	current_addr = WriteAddr;		// 获取要写入的地址
	end_addr = WriteAddr + Size;	// 计算结束地址
	write_data = pBuffer;			// 获取要写入的数据

	do
	{
		// 按页写入数据
		if( SPI_W25Qxx_WritePage(write_data, current_addr, current_size) != SPI_W25Qxx_OK )
		{
			return W25Qxx_ERROR_TRANSMIT;
		}

		else // 按页写入数据成功，进行下一次写数据的准备工作
		{
			current_addr += current_size;	// 计算下一次要写入的地址
			write_data += current_size;	// 获取下一次要写入的数据存储区地址
			// 计算下一次写数据的长度
			current_size = ((current_addr + SPI_W25Qxx_PageSize) > end_addr) ? (end_addr - current_addr) : SPI_W25Qxx_PageSize;
		}
	}
	while (current_addr < end_addr) ; // 判断数据是否全部写入完毕

	return SPI_W25Qxx_OK;	// 写入数据成功
}

/**********************************************************************************************************************************
*
*	函 数 名: SPI_W25Qxx_ReadBuffer
*
*	入口参数:  pBuffer 		  - 要读取的数据存储区
*				 ReadAddr 		 - 要读取 W25Qxx 的地址
*				 NumByteToRead  - 数据长度，最大不能超过flash芯片的大小
*
*	返 回 值: SPI_W25Qxx_OK 		     - 读数据成功
*				 W25Qxx_ERROR_TRANSMIT	  - 传输失败
*
*	函数功能: 读取数据，最大不能超过flash芯片的大小
*
*	说    明: 1.本例程使用的读指令为 0x03，SCK驱动时钟最大不能超过50M
*
*            2.这里使用由HAL库函数 HAL_SPI_TransmitReceive() 修改而得的 MY_SPI_TransmitReceive() 进行读取，
*              目的是为了SPI读取数据不用计算偏移以及不限数据长度的读取
*
*****************************************************************************************************************FANKE************/

int8_t SPI_W25Qxx_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{  
    uint8_t	SPI_TX_Buff[4];   // 要发送的指令

    SPI_TX_Buff[0] = W25Qxx_CMD_Read;                  //  读命令 
    SPI_TX_Buff[1] = ( ReadAddr & 0xFF0000 ) >> 16;    //  地址
    SPI_TX_Buff[2] = ( ReadAddr & 0xFF00 ) >> 8;;
    SPI_TX_Buff[3] =   ReadAddr & 0xFF;

	 if(MY_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)pBuffer, sizeof(SPI_TX_Buff),NumByteToRead) != HAL_OK)	
	 {
	 	return W25Qxx_ERROR_TRANSMIT;
	 }	
   // /* 使用HAL原本的库函数进行读取，需要注意 数据总长度 = 读取长度 + 发送的指令和地址长度 */
   // /* 用户在提取读到的数据时，需要偏移 指令和地址 的长度  */
   // /* 使用该函数，最大单次读取的长度不能超过64K字节  */  
//    if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)pBuffer, NumByteToRead+sizeof(SPI_TX_Buff), 1000) != HAL_OK)	
//    {
//    return W25Qxx_ERROR_TRANSMIT;
//    }	

	return SPI_W25Qxx_OK; // 写入成功
}

/**********************************************************************************************************************************
*
* 以下几个函数修改于HAL的库函数，目的是为了SPI读取数据不用计算偏移以及不限数据长度的读取
*
*****************************************************************************************************************FANKE************/

/**
  * @brief Handle SPI Communication Timeout.
  * @param hspi: pointer to a SPI_HandleTypeDef structure that contains
  *              the configuration information for SPI module.
  * @param Flag: SPI flag to check
  * @param Status: flag state to check
  * @param Timeout: Timeout duration
  * @param Tickstart: Tick start value
  * @retval HAL status
  */
HAL_StatusTypeDef MY_SPI_WaitOnFlagUntilTimeout(SPI_HandleTypeDef *hspi, uint32_t Flag, FlagStatus Status,
                                                    uint32_t Tickstart, uint32_t Timeout)
{
   /* Wait until flag is set */
   while ((__HAL_SPI_GET_FLAG(hspi, Flag) ? SET : RESET) == Status)
   {
      /* Check for the Timeout */
      if ((((HAL_GetTick() - Tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
      {
         return HAL_TIMEOUT;
      }
   }
   return HAL_OK;
}


/**
 * @brief  Close Transfer and clear flags.
 * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
 *               the configuration information for SPI module.
 * @retval HAL_ERROR: if any error detected
 *         HAL_OK: if nothing detected
 */
 void MY_SPI_CloseTransfer(SPI_HandleTypeDef *hspi)
{
  uint32_t itflag = hspi->Instance->SR;

  __HAL_SPI_CLEAR_EOTFLAG(hspi);
  __HAL_SPI_CLEAR_TXTFFLAG(hspi);

  /* Disable SPI peripheral */
  __HAL_SPI_DISABLE(hspi);

  /* Disable ITs */
  __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_EOT | SPI_IT_TXP | SPI_IT_RXP | SPI_IT_DXP | SPI_IT_UDR | SPI_IT_OVR | SPI_IT_FRE | SPI_IT_MODF));

  /* Disable Tx DMA Request */
  CLEAR_BIT(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN);

  /* Report UnderRun error for non RX Only communication */
  if (hspi->State != HAL_SPI_STATE_BUSY_RX)
  {
    if ((itflag & SPI_FLAG_UDR) != 0UL)
    {
      SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_UDR);
      __HAL_SPI_CLEAR_UDRFLAG(hspi);
    }
  }

  /* Report OverRun error for non TX Only communication */
  if (hspi->State != HAL_SPI_STATE_BUSY_TX)
  {
    if ((itflag & SPI_FLAG_OVR) != 0UL)
    {
      SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_OVR);
      __HAL_SPI_CLEAR_OVRFLAG(hspi);
    }
  }

  /* SPI Mode Fault error interrupt occurred -------------------------------*/
  if ((itflag & SPI_FLAG_MODF) != 0UL)
  {
    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_MODF);
    __HAL_SPI_CLEAR_MODFFLAG(hspi);
  }

  /* SPI Frame error interrupt occurred ------------------------------------*/
  if ((itflag & SPI_FLAG_FRE) != 0UL)
  {
    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FRE);
    __HAL_SPI_CLEAR_FREFLAG(hspi);
  }

  hspi->TxXferCount = (uint16_t)0UL;
  hspi->RxXferCount = (uint16_t)0UL;
}


/**
  * @brief  Transmit and Receive an amount of data in blocking mode.
  * @param  hspi   : pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  pTxData: pointer to transmission data buffer
  * @param  pRxData: pointer to reception data buffer
  * @param  Size   : amount of data to be sent and received
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */

HAL_StatusTypeDef MY_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint32_t TxSize,uint32_t RxSize)
{
   HAL_StatusTypeDef errorcode = HAL_OK;
   uint32_t    tickstart;
   uint32_t    Timeout = 1000;
   uint32_t    initial_TxXferCount;
   uint32_t    initial_RxXferCount;

   /* Check Direction parameter */
   assert_param(IS_SPI_DIRECTION_2LINES(hspi->Init.Direction));

   /* Process Locked */
   __HAL_LOCK(hspi);

   /* Init tickstart for timeout management*/
   tickstart = HAL_GetTick();

   /* Don't overwrite in case of HAL_SPI_STATE_BUSY_RX */
   if (hspi->State != HAL_SPI_STATE_BUSY_RX)
   {
      hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
   }

   /* Set the transaction information */
   hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
   hspi->pRxBuffPtr  = (uint8_t *)pRxData;
   hspi->pTxBuffPtr  = (uint8_t *)pTxData;

   /*Init field not used in handle to zero */
   hspi->RxISR       = NULL;
   hspi->TxISR       = NULL;

// 不使用硬件 TSIZE 控制，此处设置为0，即不限制传输的数据长度
   MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, 0);

   __HAL_SPI_ENABLE(hspi);

   if (hspi->Init.Mode == SPI_MODE_MASTER)
   {
      /* Master transfer start */
      SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
   }

 /* 先发送命令和地址，长度为 TxSize */   
   initial_TxXferCount = TxSize;
   initial_RxXferCount = TxSize;  
   hspi->pRxBuffPtr  = (uint8_t *)pRxData;
   hspi->pTxBuffPtr  = (uint8_t *)pTxData;

   while ((initial_TxXferCount > 0UL) || (initial_RxXferCount > 0UL))
   {
      /* check TXP flag */
      if ((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP)) && (initial_TxXferCount > 0UL))
      {
         *((__IO uint8_t *)&hspi->Instance->TXDR) = *((uint8_t *)hspi->pTxBuffPtr);
         hspi->pTxBuffPtr += sizeof(uint8_t);
         initial_TxXferCount--;         
      }
      /* Wait until RXWNE/FRLVL flag is reset */
      if (((hspi->Instance->SR & (SPI_FLAG_RXWNE | SPI_FLAG_FRLVL)) != 0UL) && (initial_RxXferCount > 0UL))
      {
         *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
         hspi->pRxBuffPtr += sizeof(uint8_t);
         initial_RxXferCount --;         
      }
   }

 /* 开始读取数据，长度为 RxSize */   
   initial_TxXferCount = RxSize ;
   initial_RxXferCount = RxSize ;  
   hspi->pRxBuffPtr  = (uint8_t *)pRxData;
   hspi->pTxBuffPtr  = (uint8_t *)pTxData;

   /* Transmit and Receive data in 8 Bit mode */
   while ((initial_TxXferCount > 0UL) || (initial_RxXferCount > 0UL))
   {
      /* check TXP flag */
      if ((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP)) && (initial_TxXferCount > 0UL))
      {
         if ((initial_TxXferCount > 3UL) && (hspi->Init.FifoThreshold > SPI_FIFO_THRESHOLD_03DATA))
         {
            *((__IO uint32_t *)&hspi->Instance->TXDR) = *((uint32_t *)hspi->pTxBuffPtr);
            hspi->pTxBuffPtr += sizeof(uint32_t);
            initial_TxXferCount -= (uint16_t)4UL;
         }
         else if ((initial_TxXferCount > 1UL) && (hspi->Init.FifoThreshold > SPI_FIFO_THRESHOLD_01DATA))
         {
            *((__IO uint16_t *)&hspi->Instance->TXDR) = *((uint16_t *)hspi->pTxBuffPtr);
            hspi->pTxBuffPtr += sizeof(uint16_t);
            initial_TxXferCount -= (uint16_t)2UL;         
         }
         else
         {
            *((__IO uint8_t *)&hspi->Instance->TXDR) = *((uint8_t *)hspi->pTxBuffPtr);
            hspi->pTxBuffPtr += sizeof(uint8_t);
            initial_TxXferCount--;         
         }
      }

      /* Wait until RXWNE/FRLVL flag is reset */
      if (((hspi->Instance->SR & (SPI_FLAG_RXWNE | SPI_FLAG_FRLVL)) != 0UL) && (initial_RxXferCount > 0UL))
      {
        if ((hspi->Instance->SR & SPI_FLAG_RXWNE) != 0UL)
        {
            *((uint32_t *)hspi->pRxBuffPtr) = *((__IO uint32_t *)&hspi->Instance->RXDR);
            hspi->pRxBuffPtr += sizeof(uint32_t);  
            initial_RxXferCount -= (uint16_t)4UL;		          
        }
        else if ((hspi->Instance->SR & SPI_FLAG_FRLVL) > SPI_RX_FIFO_1PACKET)
        {
            *((uint16_t *)hspi->pRxBuffPtr) = *((__IO uint16_t *)&hspi->Instance->RXDR);
            hspi->pRxBuffPtr += sizeof(uint16_t);
				initial_RxXferCount -= (uint16_t)2UL;	         
        }
        else
        {
            *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
            hspi->pRxBuffPtr += sizeof(uint8_t);
				initial_RxXferCount --;         
        }
      }
      /* Timeout management */
      if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
      {
         /* Call standard close procedure with error check */
         MY_SPI_CloseTransfer(hspi);

         /* Process Unlocked */
         __HAL_UNLOCK(hspi);

         SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
         hspi->State = HAL_SPI_STATE_READY;
			
//			printf ("\r\ninitial_RxXferCount:%d\r\n",initial_RxXferCount);
			
         return HAL_ERROR;
      }
    }

   SET_BIT((hspi)->Instance->CR1 , SPI_CR1_CSUSP); // 请求挂起SPI传输

   /* 等待SPI挂起 */
   if (MY_SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_SUSP, RESET, tickstart, Timeout) != HAL_OK)
   {
      SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
   }
   MY_SPI_CloseTransfer(hspi);   /* Call standard close procedure with error check */

   SET_BIT((hspi)->Instance->IFCR , SPI_IFCR_SUSPC);  // 清除挂起标志位
	
  /* Process Unlocked */
  __HAL_UNLOCK(hspi);

  hspi->State = HAL_SPI_STATE_READY;

  if (hspi->ErrorCode != HAL_SPI_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  return errorcode;
}

/**************************************************************************************************************************************************************************************************************************************************************************FANKE***/
// 实验平台：反客 STM32H7B0核心板
//



