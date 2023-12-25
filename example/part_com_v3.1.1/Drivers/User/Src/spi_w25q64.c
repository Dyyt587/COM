/***
	************************************************************************************************************************************************************************************************
	*	@file   	spi_w25q64.c
	*	@version V1.0
	*  @date    2021-8-4
	*	@author  ���ͿƼ�
	*	@brief   SPI ���� W25Qxxϵ����غ���
   **********************************************************************************************************************************************************************************************
   *  @description
	*
	*	ʵ��ƽ̨������STM32H7B0VBT6���İ� ���ͺţ�FK7B0M1-VBT6����ʹ�õ���W25Q64
	*	�Ա���ַ��https://shop212360197.taobao.com
	*	QQ����Ⱥ��536665479
	*
>>>>> �ļ�˵����
	*
	*  1. ʹ��Ӳ�� SPI6 ���� W25Q64
	*  2. �ṩ�Ķ�д������ʹ��HAL�⺯��ֱ�Ӳ�����û���õ�DMA���ж�
   *  3. SPI6 SCK����ʱ������Ϊ 50MHz
   *
>>>>> ��Ҫ˵����
	*
	*	1. W25Q64JV�Ĳ���ʱ�����޶���!!! �ֲ�����ĵ��Ͳο�ֵΪ: 4K-45ms, 32K-120ms ,64K-150ms,��Ƭ����20S
	*
	*	2. W25Q64JV��д��ʱ�����޶���!!! �ֲ�����ĵ��Ͳο�ֵΪ: 256�ֽ�-0.4ms��Ҳ���� 1M�ֽ�/s ��ʵ������600K�ֽ�/s���ң�
	*
	*  3. W25Q64JV��������������Ƶ����Ȼ��ֹ50MHz����ʹ�ö�ȡָ�� 0x03 �����������ٶ�ֻ�ܵ�50M
	* 
	*	4. ʵ��ʹ���У������ݱȽϴ�ʱ������ʹ��64K����32K����������ʱ���4K������	
	*
	*********************************************************************************************************************************************************************************************FANKE*****
***/

#include "spi_w25q64.h"
#include "stdio.h"
#include "spi.h"

//SPI_HandleTypeDef hspi6;	// SPI_HandleTypeDef �ṹ�����

/*************************************************************************************************
*	�� �� ��:	HAL_SPI_MspInit
*	��ڲ���:	hspi - SPI_HandleTypeDef����ı���������ʾ����� SPI ���
*	�� �� ֵ:	��
*	��������:	��ʼ�� SPI6 ����
*	˵    ��:	��		
*************************************************************************************************/

/*************************************************************************************************
*	�� �� ��:	MX_SPI6_Init
*	��ڲ���:	��
*	�� �� ֵ:	��
*	��������:	��ʼ��SPI����
*	˵    ��:	ʹ��Ӳ��Ƭѡ	 
*************************************************************************************************/

 
/*************************************************************************************************
*	�� �� ��: SPI_W25Qxx_Init
*	��ڲ���: ��
*	�� �� ֵ: SPI_W25Qxx_OK - ��ʼ���ɹ���W25Qxx_ERROR_INIT - ��ʼ������
*	��������: ��ʼ�� SPI ���ã���ȡW25Q64ID
*	˵    ��: ��	
*************************************************************************************************/

int8_t	SPI_W25Qxx_Init(void)
{
 	uint32_t	Device_ID;	// ��������洢����ID

	//MX_SPI6_Init();  							 		// ��ʼ�� SPI6 ����
   Device_ID =  SPI_W25Qxx_ReadID();			// ��ȡID

	if( Device_ID == SPI_W25Qxx_FLASH_ID )		// ����ƥ��
	{
		printf ("W25Q64 OK,flash ID:%X\r\n",Device_ID);		// ��ʼ���ɹ�
		return SPI_W25Qxx_OK;			// ���سɹ���־		
	}
	else
	{
		printf ("W25Q64 ERROR!!!!!  ID:%X\r\n",Device_ID);	// ��ʼ��ʧ��	
		return W25Qxx_ERROR_INIT;		// ���ش����־
	}	
}

/*************************************************************************************************
*	�� �� ��: SPI_W25Qxx_ReadID
*	��ڲ���: ��
*	�� �� ֵ: W25Qxx_ID - ��ȡ��������ID��W25Qxx_ERROR_INIT - ͨ�š���ʼ������
*	��������: ��ȡ����ID
*	˵    ��: ��	
**************************************************************************************************/

uint32_t SPI_W25Qxx_ReadID(void)
{
   uint8_t	SPI_TX_Buff[1] __attribute__ ((at(0x38000000)));      // Ҫ���͵�ָ��
	uint8_t	SPI_RX_Buff[4] __attribute__ ((at(0X38000100)));		// �洢SPI����������
   uint32_t W25Qxx_ID;           // ID
	SPI_TX_Buff[0] = SPI_W25Qxx_CMD_JedecID;		// ���Ͷ�JedecID���� 
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

// ��Ҫע��ģ�ʹ�� HAL_SPI_TransmitReceive() ����ͨ�ţ����͵�һ����ID����ʱ���ͻ�������ȡһ���ֽ�
// ��ˣ�Ҫ��õ�ʵ�ʵ�������Ҫƫ��һ���ֽ�
   W25Qxx_ID = (SPI_RX_Buff[1] << 16) | (SPI_RX_Buff[2] << 8) | SPI_RX_Buff[3];    // ���������������ΪID

   return W25Qxx_ID; // ����IDֵ
}

/*************************************************************************************************
*	�� �� ��: SPI_W25Qxx_WaitForWriteEnd  
*	��ڲ���: ��
*	�� �� ֵ: SPI_W25Qxx_OK - ͨ������������W25Qxx_ERROR_Wait - �ȴ�����
*	��������: �ȴ�W25Qxx�������
*	˵    ��: ����/д������ʱ����Ҫ���øú����ȴ��������	
**************************************************************************************************/

int8_t SPI_W25Qxx_WaitForWriteEnd(void)
{
   uint8_t	SPI_TX_Buff[1];   // Ҫ���͵�ָ��
	uint8_t	SPI_RX_Buff[2];	// �洢SPI���������� 

   while (1)
   {
	   SPI_TX_Buff[0] = SPI_W25Qxx_CMD_ReadStatus_REG1;		// ���Ͷ� ��״̬�Ĵ��� ���� 
      SPI_RX_Buff[1] = 0;     // ���洢������

      if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)SPI_RX_Buff, 2, 1000) != HAL_OK)	
      {
         return W25Qxx_ERROR_Wait;
      }	  

// ��״̬�Ĵ���1�ĵ�0λ��ֻ������Busy��־λ�������ڲ���/д������/д����ʱ�ᱻ��1
      if( (SPI_RX_Buff[1] & SPI_W25Qxx_CMD_Status_REG1_BUSY)  != SET )  // �Ƚϱ�־λ
      {
         return SPI_W25Qxx_OK;
      }   
   }
}

/*************************************************************************************************
*	�� �� ��: SPI_W25Qxx_WriteEnable
*	��ڲ���: ��
*	�� �� ֵ: QSPI_W25Qxx_OK - дʹ�ܳɹ���W25Qxx_ERROR_WriteEnable - дʹ��ʧ��
*	��������: ����дʹ������
*	˵    ��: �ڲ�������д������֮ǰ����Ҫ���øú�������дʹ��
**************************************************************************************************/

int8_t SPI_W25Qxx_WriteEnable(void)
{
   uint8_t	SPI_TX_Buff[1];   // Ҫ���͵�ָ��
	uint8_t	SPI_RX_Buff[1];	// �洢SPI���������ݣ��˴��ò��� 

	SPI_TX_Buff[0] = SPI_W25Qxx_CMD_WriteEnable;		// ���Ͷ� дʹ�� ���� 

	if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)SPI_RX_Buff, 1, 1000) != HAL_OK)	
	{
		return W25Qxx_ERROR_WriteEnable;
	}	
   return SPI_W25Qxx_OK;
}

/*************************************************************************************************
*
*	�� �� ��: SPI_W25Qxx_SectorErase
*
*	��ڲ���: SectorAddress - Ҫ�����ĵ�ַ
*
*	�� �� ֵ: SPI_W25Qxx_OK - �����ɹ�
*			    W25Qxx_ERROR_Erase - ����ʧ��
*
*	��������: ������������������ÿ�β���4K�ֽ�
*
*	˵    ��: 1.���� W25Q64JV �����ֲ�����Ĳ����ο�ʱ�䣬����ֵΪ 45ms�����ֵΪ400ms
*				 2.ʵ�ʵĲ����ٶȿ��ܴ���45ms��Ҳ����С��45ms
*				 3.flashʹ�õ�ʱ��Խ������������ʱ��Ҳ��Խ��
*
**************************************************************************************************/

int8_t SPI_W25Qxx_SectorErase(uint32_t SectorAddress)	
{
   uint8_t	SPI_TX_Buff[4];   // Ҫ���͵�ָ��
	uint8_t	SPI_RX_Buff[1];	// �洢SPI���������ݣ��˴��ò��� 

   SPI_W25Qxx_WriteEnable();  // дʹ��

   SPI_TX_Buff[0] = SPI_W25Qxx_CMD_SectorErase;          // 4K��������ָ��
   SPI_TX_Buff[1] = ( SectorAddress & 0xFF0000 ) >> 16;  // ����Ҫ�����ĵ�ַ
   SPI_TX_Buff[2] = ( SectorAddress & 0xFF00 ) >> 8;;
   SPI_TX_Buff[3] =   SectorAddress & 0xFF;

	if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)SPI_RX_Buff, 4, 1000) != HAL_OK)	
	{
		return W25Qxx_ERROR_Erase;
	}	
   
   SPI_W25Qxx_WaitForWriteEnd(); // �ȴ���������

	return SPI_W25Qxx_OK; // �����ɹ�
}

/*************************************************************************************************
*
*	�� �� ��: SPI_W25Qxx_BlockErase_32K
*
*	��ڲ���: SectorAddress - Ҫ�����ĵ�ַ
*
*	�� �� ֵ: SPI_W25Qxx_OK - �����ɹ�
*			    W25Qxx_ERROR_Erase - ����ʧ��
*
*	��������: ���п����������ÿ�β���32K�ֽ�
*
*	˵    ��: 1.���� W25Q64JV �����ֲ�����Ĳ����ο�ʱ�䣬����ֵΪ 120ms�����ֵΪ1600ms
*				 2.ʵ�ʵĲ����ٶȿ��ܴ���120ms��Ҳ����С��120ms
*				 3.flashʹ�õ�ʱ��Խ������������ʱ��Ҳ��Խ��
*
*************************************************************************************************/

int8_t SPI_W25Qxx_BlockErase_32K(uint32_t SectorAddress)	
{
   uint8_t	SPI_TX_Buff[4];   // Ҫ���͵�ָ��
	uint8_t	SPI_RX_Buff[1];	// �洢SPI���������ݣ��˴��ò��� 

   SPI_W25Qxx_WriteEnable();  // дʹ��

   SPI_TX_Buff[0] = SPI_W25Qxx_CMD_BlockErase_32K;       // 32K�����ָ��
   SPI_TX_Buff[1] = ( SectorAddress & 0xFF0000 ) >> 16;  // ����Ҫ�����ĵ�ַ
   SPI_TX_Buff[2] = ( SectorAddress & 0xFF00 ) >> 8;;
   SPI_TX_Buff[3] =   SectorAddress & 0xFF;

	if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)SPI_RX_Buff, 4, 1000) != HAL_OK)	
	{
		return W25Qxx_ERROR_Erase;
	}	
   
   SPI_W25Qxx_WaitForWriteEnd(); // �ȴ��������

	return SPI_W25Qxx_OK; // �����ɹ�
}

/*************************************************************************************************
*
*	�� �� ��: SPI_W25Qxx_BlockErase_64K
*
*	��ڲ���: SectorAddress - Ҫ�����ĵ�ַ
*
*	�� �� ֵ: SPI_W25Qxx_OK - �����ɹ�
*			    W25Qxx_ERROR_Erase - ����ʧ��
*
*	��������: ���п����������ÿ�β���64K�ֽ�
*
*	˵    ��: 1.���� W25Q64JV �����ֲ�����Ĳ����ο�ʱ�䣬����ֵΪ 150ms�����ֵΪ2000ms
*				 2.ʵ�ʵĲ����ٶȿ��ܴ���150ms��Ҳ����С��150ms
*				 3.flashʹ�õ�ʱ��Խ������������ʱ��Ҳ��Խ��
*				 4.ʵ��ʹ�ý���ʹ��64K������������ʱ�����
*
**************************************************************************************************/

int8_t SPI_W25Qxx_BlockErase_64K(uint32_t SectorAddress)	
{
   uint8_t	SPI_TX_Buff[4];   // Ҫ���͵�ָ��
	uint8_t	SPI_RX_Buff[1];	// �洢SPI���������ݣ��˴��ò��� 

   SPI_W25Qxx_WriteEnable();  // дʹ��   

   SPI_TX_Buff[0] = SPI_W25Qxx_CMD_BlockErase_64K;       // 64K�����ָ��
   SPI_TX_Buff[1] = ( SectorAddress & 0xFF0000 ) >> 16;  // ����Ҫ�����ĵ�ַ
   SPI_TX_Buff[2] = ( SectorAddress & 0xFF00 ) >> 8;;
   SPI_TX_Buff[3] =   SectorAddress & 0xFF;

	if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)SPI_RX_Buff, 4, 1000) != HAL_OK)	
	{
		return W25Qxx_ERROR_Erase;
	}	
   
   SPI_W25Qxx_WaitForWriteEnd();    // �ȴ���������

	return SPI_W25Qxx_OK; // �����ɹ�
}

/*************************************************************************************************
*
*	�� �� ��: SPI_W25Qxx_ChipErase
*
*	��ڲ���: ��
*
*	�� �� ֵ: SPI_W25Qxx_OK - �����ɹ�
*			    W25Qxx_ERROR_Erase - ����ʧ��
*
*	��������: ������Ƭ��������
*
*	˵    ��: 1.���� W25Q64JV �����ֲ�����Ĳ����ο�ʱ�䣬����ֵΪ 20s�����ֵΪ100s
*				 2.ʵ�ʵĲ����ٶȿ��ܴ���20s��Ҳ����С��20s
*				 3.flashʹ�õ�ʱ��Խ������������ʱ��Ҳ��Խ��
*
*************************************************************************************************/

int8_t SPI_W25Qxx_ChipErase(void)	
{
   uint8_t	SPI_TX_Buff[1];   // Ҫ���͵�ָ��
	uint8_t	SPI_RX_Buff[1];	// �洢SPI���������ݣ��˴��ò��� 

   SPI_W25Qxx_WriteEnable();  // дʹ��

   SPI_TX_Buff[0] = SPI_W25Qxx_CMD_ChipErase;   // ȫƬ����ָ��

	if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)SPI_RX_Buff, 1, 1000) != HAL_OK)	
	{
		return W25Qxx_ERROR_Erase;
	}	
   
   SPI_W25Qxx_WaitForWriteEnd();

	return SPI_W25Qxx_OK; // �����ɹ�
}

/**********************************************************************************************************
*
*	�� �� ��: SPI_W25Qxx_WritePage
*
*	��ڲ���: pBuffer 		 - Ҫд�������
*				 WriteAddr 		 - Ҫд�� W25Qxx �ĵ�ַ
*				 NumByteToWrite - ���ݳ��ȣ����ֻ��256�ֽ�
*
*	�� �� ֵ: SPI_W25Qxx_OK 		     - д���ݳɹ�
*				 W25Qxx_ERROR_TRANSMIT	  - ����ʧ��
*
*	��������: ��ҳд�룬���ֻ��256�ֽڣ�������д��֮ǰ���������ɲ�������
*
*	˵    ��: 1.Flash��д��ʱ�����޶���
*				 2.���� W25Q64JV �����ֲ������ ҳ(256�ֽ�) д��ο�ʱ�䣬����ֵΪ 0.4ms�����ֵΪ3ms
*				 3.ʵ�ʵ�д���ٶȿ��ܴ���0.4ms��Ҳ����С��0.4ms
*				 4.Flashʹ�õ�ʱ��Խ����д������ʱ��Ҳ��Խ��
*				 5.������д��֮ǰ���������ɲ�������
*
***********************************************************************************************************/

int8_t SPI_W25Qxx_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t	SPI_TX_Buff[260];    // Ҫ���͵�ָ��	
	uint16_t i = 0;               // ��������

   SPI_W25Qxx_WriteEnable();     // дʹ��

   SPI_TX_Buff[0] = W25Qxx_CMD_PageProgram;           // ҳ���ָ��
   SPI_TX_Buff[1] = ( WriteAddr & 0xFF0000 ) >> 16;   // ��ַ
   SPI_TX_Buff[2] = ( WriteAddr & 0xFF00 ) >> 8;;
   SPI_TX_Buff[3] =   WriteAddr & 0xFF;

   for ( i = 0; i < NumByteToWrite; i++)  // ����Ҫд�������
   {
      SPI_TX_Buff[4+i] = * pBuffer;
      pBuffer++;
   }

	if( HAL_SPI_Transmit(&hspi6, (uint8_t *)SPI_TX_Buff, NumByteToWrite+4, 1000)!= HAL_OK )	// ��ʼ����
	{
		return W25Qxx_ERROR_TRANSMIT;
	}	
   
   SPI_W25Qxx_WaitForWriteEnd(); // �ȴ�д�����

	return SPI_W25Qxx_OK; // д��ɹ�
}

/**********************************************************************************************************
*
*	�� �� ��: SPI_W25Qxx_WriteBuffer
*
*	��ڲ���: pBuffer 		 - Ҫд�������
*				 WriteAddr 		 - Ҫд�� W25Qxx �ĵ�ַ
*				 NumByteToWrite - ���ݳ��ȣ�����ܳ���flashоƬ�Ĵ�С
*
*	�� �� ֵ: QSPI_W25Qxx_OK 		     - д���ݳɹ�
*			    W25Qxx_ERROR_WriteEnable - дʹ��ʧ��
*				 W25Qxx_ERROR_TRANSMIT	  - ����ʧ��
*
*	��������: д�����ݣ�����ܳ���flashоƬ�Ĵ�С��д����ǰ�������ɲ�������
*
*	˵    ��: 1.Flash��д��ʱ�������޶���
*				 2.���� W25Q64JV �����ֲ������ ҳ д��ο�ʱ�䣬����ֵΪ 0.4ms�����ֵΪ3ms
*				 3.ʵ�ʵ�д���ٶȿ��ܴ���0.4ms��Ҳ����С��0.4ms
*				 4.Flashʹ�õ�ʱ��Խ����д������ʱ��Ҳ��Խ��
*				 5.������д��֮ǰ���������ɲ�������
*				 6.�ú�����ֲ�� stm32h743i_eval_qspi.c
*
**********************************************************************************************************/

int8_t SPI_W25Qxx_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t Size)
{	
	uint32_t end_addr=0, current_size=0, current_addr=0;
	uint8_t *write_data;  // Ҫд�������

	current_size = SPI_W25Qxx_PageSize - (WriteAddr % SPI_W25Qxx_PageSize); // ���㵱ǰҳ��ʣ��Ŀռ�

	if (current_size > Size)	// �жϵ�ǰҳʣ��Ŀռ��Ƿ��㹻д����������
	{
		current_size = Size;		// ����㹻����ֱ�ӻ�ȡ��ǰ����
	}

	current_addr = WriteAddr;		// ��ȡҪд��ĵ�ַ
	end_addr = WriteAddr + Size;	// ���������ַ
	write_data = pBuffer;			// ��ȡҪд�������

	do
	{
		// ��ҳд������
		if( SPI_W25Qxx_WritePage(write_data, current_addr, current_size) != SPI_W25Qxx_OK )
		{
			return W25Qxx_ERROR_TRANSMIT;
		}

		else // ��ҳд�����ݳɹ���������һ��д���ݵ�׼������
		{
			current_addr += current_size;	// ������һ��Ҫд��ĵ�ַ
			write_data += current_size;	// ��ȡ��һ��Ҫд������ݴ洢����ַ
			// ������һ��д���ݵĳ���
			current_size = ((current_addr + SPI_W25Qxx_PageSize) > end_addr) ? (end_addr - current_addr) : SPI_W25Qxx_PageSize;
		}
	}
	while (current_addr < end_addr) ; // �ж������Ƿ�ȫ��д�����

	return SPI_W25Qxx_OK;	// д�����ݳɹ�
}

/**********************************************************************************************************************************
*
*	�� �� ��: SPI_W25Qxx_ReadBuffer
*
*	��ڲ���:  pBuffer 		  - Ҫ��ȡ�����ݴ洢��
*				 ReadAddr 		 - Ҫ��ȡ W25Qxx �ĵ�ַ
*				 NumByteToRead  - ���ݳ��ȣ�����ܳ���flashоƬ�Ĵ�С
*
*	�� �� ֵ: SPI_W25Qxx_OK 		     - �����ݳɹ�
*				 W25Qxx_ERROR_TRANSMIT	  - ����ʧ��
*
*	��������: ��ȡ���ݣ�����ܳ���flashоƬ�Ĵ�С
*
*	˵    ��: 1.������ʹ�õĶ�ָ��Ϊ 0x03��SCK����ʱ������ܳ���50M
*
*            2.����ʹ����HAL�⺯�� HAL_SPI_TransmitReceive() �޸Ķ��õ� MY_SPI_TransmitReceive() ���ж�ȡ��
*              Ŀ����Ϊ��SPI��ȡ���ݲ��ü���ƫ���Լ��������ݳ��ȵĶ�ȡ
*
*****************************************************************************************************************FANKE************/

int8_t SPI_W25Qxx_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{  
    uint8_t	SPI_TX_Buff[4];   // Ҫ���͵�ָ��

    SPI_TX_Buff[0] = W25Qxx_CMD_Read;                  //  ������ 
    SPI_TX_Buff[1] = ( ReadAddr & 0xFF0000 ) >> 16;    //  ��ַ
    SPI_TX_Buff[2] = ( ReadAddr & 0xFF00 ) >> 8;;
    SPI_TX_Buff[3] =   ReadAddr & 0xFF;

	 if(MY_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)pBuffer, sizeof(SPI_TX_Buff),NumByteToRead) != HAL_OK)	
	 {
	 	return W25Qxx_ERROR_TRANSMIT;
	 }	
   // /* ʹ��HALԭ���Ŀ⺯�����ж�ȡ����Ҫע�� �����ܳ��� = ��ȡ���� + ���͵�ָ��͵�ַ���� */
   // /* �û�����ȡ����������ʱ����Ҫƫ�� ָ��͵�ַ �ĳ���  */
   // /* ʹ�øú�������󵥴ζ�ȡ�ĳ��Ȳ��ܳ���64K�ֽ�  */  
//    if(HAL_SPI_TransmitReceive(&hspi6, (uint8_t *)SPI_TX_Buff, (uint8_t *)pBuffer, NumByteToRead+sizeof(SPI_TX_Buff), 1000) != HAL_OK)	
//    {
//    return W25Qxx_ERROR_TRANSMIT;
//    }	

	return SPI_W25Qxx_OK; // д��ɹ�
}

/**********************************************************************************************************************************
*
* ���¼��������޸���HAL�Ŀ⺯����Ŀ����Ϊ��SPI��ȡ���ݲ��ü���ƫ���Լ��������ݳ��ȵĶ�ȡ
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

// ��ʹ��Ӳ�� TSIZE ���ƣ��˴�����Ϊ0���������ƴ�������ݳ���
   MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, 0);

   __HAL_SPI_ENABLE(hspi);

   if (hspi->Init.Mode == SPI_MODE_MASTER)
   {
      /* Master transfer start */
      SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
   }

 /* �ȷ�������͵�ַ������Ϊ TxSize */   
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

 /* ��ʼ��ȡ���ݣ�����Ϊ RxSize */   
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

   SET_BIT((hspi)->Instance->CR1 , SPI_CR1_CSUSP); // �������SPI����

   /* �ȴ�SPI���� */
   if (MY_SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_SUSP, RESET, tickstart, Timeout) != HAL_OK)
   {
      SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
   }
   MY_SPI_CloseTransfer(hspi);   /* Call standard close procedure with error check */

   SET_BIT((hspi)->Instance->IFCR , SPI_IFCR_SUSPC);  // ��������־λ
	
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
// ʵ��ƽ̨������ STM32H7B0���İ�
//



