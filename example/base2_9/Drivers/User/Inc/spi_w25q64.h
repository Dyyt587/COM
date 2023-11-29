#ifndef spi_w25q64
#define spi_w25q64

#include "stm32h7xx_hal.h"
#include "usart.h"

/*----------------------------------------------- ���������� -------------------------------------------*/

#define SPI_W25Qxx_OK           		    0		// W25Qxxͨ������
#define W25Qxx_ERROR_INIT         		-1		// ��ʼ������
#define W25Qxx_ERROR_WriteEnable       -2		// дʹ�ܴ���
#define W25Qxx_ERROR_Wait              -3		// ��ѯ�ȴ���������Ӧ
#define W25Qxx_ERROR_Erase         		-4		// ��������
#define W25Qxx_ERROR_TRANSMIT         	-5		// �������
#define W25Qxx_ERROR_MemoryMapped		-6    // �ڴ�ӳ��ģʽ����


#define SPI_W25Qxx_CMD_JedecID 			      0x9F		   // JEDEC ID  
#define SPI_W25Qxx_CMD_WriteEnable		      0X06		   // дʹ��

#define SPI_W25Qxx_CMD_SectorErase 		      0x20		   // ����������4K�ֽڣ� �ο�����ʱ�� 45ms
#define SPI_W25Qxx_CMD_BlockErase_32K 	      0x52		   // �������  32K�ֽڣ��ο�����ʱ�� 120ms
#define SPI_W25Qxx_CMD_BlockErase_64K 	      0xD8		   // �������  64K�ֽڣ��ο�����ʱ�� 150ms
#define SPI_W25Qxx_CMD_ChipErase 		      0xC7		   // ��Ƭ�������ο�����ʱ�� 20S

#define W25Qxx_CMD_PageProgram  	            0x02  		// ҳ���ָ��ο�д��ʱ�� 0.4ms 
#define W25Qxx_CMD_Read       	         	0x03  		// ������ָ�ʹ�ø�ָ��ʱ�����ֻ��50MHz

#define SPI_W25Qxx_CMD_ReadStatus_REG1			0X05			// ��״̬�Ĵ���1
#define SPI_W25Qxx_CMD_Status_REG1_BUSY  		0x01			// ��״̬�Ĵ���1�ĵ�0λ��ֻ������Busy��־λ�������ڲ���/д������/д����ʱ�ᱻ��1
#define SPI_W25Qxx_CMD_Status_REG1_WEL  		0x02			// ��״̬�Ĵ���1�ĵ�1λ��ֻ������WELдʹ�ܱ�־λ���ñ�־λΪ1ʱ��������Խ���д����

#define SPI_W25Qxx_PageSize       				256			// ҳ��С��256�ֽ�
#define SPI_W25Qxx_FlashSize       				0x800000		// W25Q64��С��8M�ֽ�
#define SPI_W25Qxx_FLASH_ID           			0Xef4017    // W25Q64 JEDEC ID

/*------------------------------------------------ �������� ----------------------------------------------*/

int8_t SPI_W25Qxx_Init(void);       // W25Qxx ��ʼ��
uint32_t SPI_W25Qxx_ReadID(void);   // ��ȡ����ID

int8_t SPI_W25Qxx_WaitForWriteEnd(void);  // �ȴ� ����/д������/д���� ���
int8_t SPI_W25Qxx_WriteEnable(void);      // дʹ��

int8_t SPI_W25Qxx_SectorErase(uint32_t SectorAddress)	;     // ����������4K�ֽڣ� �ο�����ʱ�� 45ms
int8_t SPI_W25Qxx_BlockErase_32K(uint32_t SectorAddress)	;  // �������  32K�ֽڣ��ο�����ʱ�� 120ms
int8_t SPI_W25Qxx_BlockErase_64K(uint32_t SectorAddress)	;  // �������  64K�ֽڣ��ο�����ʱ�� 150ms
int8_t SPI_W25Qxx_ChipErase(void);                          // ��Ƭ�������ο�����ʱ�� 20S

int8_t SPI_W25Qxx_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);  // ҳд�룬���256�ֽڣ��ο�д��ʱ�� 0.4ms 
int8_t SPI_W25Qxx_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t Size);          // д�����ݣ�����ܳ���flashоƬ�Ĵ�С��д����ǰ�������ɲ�������
int8_t SPI_W25Qxx_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);   // ��ȡ���ݣ�����ܳ���flashоƬ�Ĵ�С

// �ú����޸���HAL�Ŀ⺯����Ŀ����Ϊ��SPI��ȡ���ݲ��ü���ƫ���Լ��������ݳ��ȵĶ�ȡ���� SPI_W25Qxx_ReadBuffer() ����
HAL_StatusTypeDef MY_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint32_t TxSize,uint32_t RxSize);
                                          
#endif //spi_w25q64




