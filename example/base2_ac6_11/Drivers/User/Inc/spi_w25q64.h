#ifndef spi_w25q64
#define spi_w25q64

#include "stm32h7xx_hal.h"
#include "usart.h"

/*----------------------------------------------- 命名参数宏 -------------------------------------------*/

#define SPI_W25Qxx_OK           		    0		// W25Qxx通信正常
#define W25Qxx_ERROR_INIT         		-1		// 初始化错误
#define W25Qxx_ERROR_WriteEnable       -2		// 写使能错误
#define W25Qxx_ERROR_Wait              -3		// 轮询等待错误，无响应
#define W25Qxx_ERROR_Erase         		-4		// 擦除错误
#define W25Qxx_ERROR_TRANSMIT         	-5		// 传输错误
#define W25Qxx_ERROR_MemoryMapped		-6    // 内存映射模式错误


#define SPI_W25Qxx_CMD_JedecID 			      0x9F		   // JEDEC ID  
#define SPI_W25Qxx_CMD_WriteEnable		      0X06		   // 写使能

#define SPI_W25Qxx_CMD_SectorErase 		      0x20		   // 扇区擦除，4K字节， 参考擦除时间 45ms
#define SPI_W25Qxx_CMD_BlockErase_32K 	      0x52		   // 块擦除，  32K字节，参考擦除时间 120ms
#define SPI_W25Qxx_CMD_BlockErase_64K 	      0xD8		   // 块擦除，  64K字节，参考擦除时间 150ms
#define SPI_W25Qxx_CMD_ChipErase 		      0xC7		   // 整片擦除，参考擦除时间 20S

#define W25Qxx_CMD_PageProgram  	            0x02  		// 页编程指令，参考写入时间 0.4ms 
#define W25Qxx_CMD_Read       	         	0x03  		// 读数据指令，使用该指令时钟最快只能50MHz

#define SPI_W25Qxx_CMD_ReadStatus_REG1			0X05			// 读状态寄存器1
#define SPI_W25Qxx_CMD_Status_REG1_BUSY  		0x01			// 读状态寄存器1的第0位（只读），Busy标志位，当正在擦除/写入数据/写命令时会被置1
#define SPI_W25Qxx_CMD_Status_REG1_WEL  		0x02			// 读状态寄存器1的第1位（只读），WEL写使能标志位，该标志位为1时，代表可以进行写操作

#define SPI_W25Qxx_PageSize       				256			// 页大小，256字节
#define SPI_W25Qxx_FlashSize       				0x800000		// W25Q64大小，8M字节
#define SPI_W25Qxx_FLASH_ID           			0Xef4017    // W25Q64 JEDEC ID

/*------------------------------------------------ 函数声明 ----------------------------------------------*/

int8_t SPI_W25Qxx_Init(void);       // W25Qxx 初始化
uint32_t SPI_W25Qxx_ReadID(void);   // 读取器件ID

int8_t SPI_W25Qxx_WaitForWriteEnd(void);  // 等待 擦除/写入数据/写命令 完成
int8_t SPI_W25Qxx_WriteEnable(void);      // 写使能

int8_t SPI_W25Qxx_SectorErase(uint32_t SectorAddress)	;     // 扇区擦除，4K字节， 参考擦除时间 45ms
int8_t SPI_W25Qxx_BlockErase_32K(uint32_t SectorAddress)	;  // 块擦除，  32K字节，参考擦除时间 120ms
int8_t SPI_W25Qxx_BlockErase_64K(uint32_t SectorAddress)	;  // 块擦除，  64K字节，参考擦除时间 150ms
int8_t SPI_W25Qxx_ChipErase(void);                          // 整片擦除，参考擦除时间 20S

int8_t SPI_W25Qxx_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);  // 页写入，最大256字节，参考写入时间 0.4ms 
int8_t SPI_W25Qxx_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t Size);          // 写入数据，最大不能超过flash芯片的大小，写数据前请务必完成擦除操作
int8_t SPI_W25Qxx_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);   // 读取数据，最大不能超过flash芯片的大小

// 该函数修改于HAL的库函数，目的是为了SPI读取数据不用计算偏移以及不限数据长度的读取，被 SPI_W25Qxx_ReadBuffer() 调用
HAL_StatusTypeDef MY_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint32_t TxSize,uint32_t RxSize);
                                          
#endif //spi_w25q64




