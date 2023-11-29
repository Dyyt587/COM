/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2016-04-23
 */

#include <sfud.h>
#include <stdarg.h>
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal.h"
#include "com_component.h"

#define LOG_TAG    "sfud"
#include "elog.h"
#include "spi.h"
#include "gpio.h"
#include "main.h"
#include "octospi.h"
#include "cmsis_os.h"

static char log_buf[256];
void sfud_log_debug(const char *file, const long line, const char *format, ...);
void sfud_log_info(const char *format, ...);
typedef struct{
	SPI_HandleTypeDef* hspi;
	OSPI_HandleTypeDef* hospi;
	void(*cs_on)(void);
	void(*cs_off)(void);
}sfud_userdata_t;

static int a =0;
static int b =0;
com_t com_spi;


void delay_1ms(void)
{
	osDelay(1);
}

static sfud_err spi_write_read_qspi(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
        size_t read_size){return SFUD_ERR_NOT_FOUND;}
/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
        size_t read_size) {
    sfud_err result = SFUD_SUCCESS;
 
		sfud_userdata_t* userdata= spi->user_data; 
    /**
     * add your spi write and read code
     */
		//HAL_GPIO_WritePin(W25Q_NSS_GPIO_Port,W25Q_NSS_Pin,GPIO_PIN_RESET);
    SFUD_ASSERT(userdata);
    SFUD_ASSERT(userdata->hspi);
    SFUD_ASSERT(userdata->cs_on);
    SFUD_ASSERT(userdata->cs_off);
		userdata->cs_on();
					//log_v("write_size=%d,read_size=%d",write_size,read_size);
					//log_v("read_size=%d",read_size);
		//vTaskEndScheduler();

    if (write_size) {
	    SFUD_ASSERT(write_buf);
						if(write_size>(1<<15))log_w("write size too big");

//		  if(HAL_SPI_Transmit(userdata->hspi,write_buf,write_size,1000)!= HAL_OK)
//			{
//				log_e("spi flash transmit error");
//			}
			
//			if(HAL_SPI_Transmit_IT(userdata->hspi,write_buf,write_size)!= HAL_OK)
//			{
//				log_e("spi flash transmit error");
//			}
			b=1;
				com_spi_tx(&com_spi,0,(uint8_t*)write_buf,write_size,COM_DELAY_EVERY);
			
	
		//	while(b);
    }
    if (read_size) {
			SFUD_ASSERT(read_buf);
			if(read_size>(1<<15))log_w("receive size too big");

//			a=1;
//				HAL_SPI_Receive_IT(com_spi.userdata, read_buf, read_size);
//			while(a);

			com_spi_start_rx(&com_spi,0,read_size,COM_DELAY_EVERY);
			
			int data = com_rx_get(&com_spi,read_buf,read_size,COM_DELAY_EVERY);
//			log_w("data = %d,read_size = %d\n",data,read_size);

//			if(HAL_SPI_Receive(userdata->hspi,read_buf,read_size,1000)!=HAL_OK)
//			{
//					log_e("spi flash receive error");
//					log_e("read_size = %d",read_size);
//			}
    }
		userdata->cs_off();
		//HAL_GPIO_WritePin(W25Q_NSS_GPIO_Port,W25Q_NSS_Pin,GPIO_PIN_SET);
		//vTaskStartScheduler();

    return result;
}
 void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	com_spi_tx_cplt(&com_spi);
		b=0;
}
 void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	//com_spi_rx_cplt(&com_spi,com_spi.task_1.len);
	a=0;

}
 void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{

}
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef * hspi)
{
    log_e("SPI ERROR E=%x\n",hspi->ErrorCode);
    hspi->ErrorCode = HAL_SPI_ERROR_NONE;
}

void spi_lock(const struct __sfud_spi *spi)
{
//vTaskEndScheduler();

}
void spi_unlock(const struct __sfud_spi *spi)
{
//vTaskStartScheduler();

}
#ifdef SFUD_USING_QSPI
/**
 * read flash data by QSPI
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format,
        uint8_t *read_buf, size_t read_size) {
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your qspi read flash data code
     */
		sfud_userdata_t* userdata= spi->user_data; 

    OSPI_RegularCmdTypeDef Cmdhandler;
    OSPI_HandleTypeDef* hospi=userdata->hospi;

    /* set cmd struct */
    Cmdhandler.Instruction = qspi_read_cmd_format->instruction;
    if(qspi_read_cmd_format->instruction_lines == 0)
    {
        Cmdhandler.InstructionMode = HAL_OSPI_INSTRUCTION_NONE;
    }else if(qspi_read_cmd_format->instruction_lines == 1)
    {
        Cmdhandler.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
    }else if(qspi_read_cmd_format->instruction_lines == 2)
    {
        Cmdhandler.InstructionMode = HAL_OSPI_INSTRUCTION_2_LINES;
    }else if(qspi_read_cmd_format->instruction_lines == 4)
    {
        Cmdhandler.InstructionMode = HAL_OSPI_INSTRUCTION_4_LINES;
    }
    
    Cmdhandler.Address = addr;
    Cmdhandler.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
    if(qspi_read_cmd_format->address_lines == 0)
    {
        Cmdhandler.AddressMode = HAL_OSPI_ADDRESS_NONE;
    }else if(qspi_read_cmd_format->address_lines == 1)
    {
        Cmdhandler.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
    }else if(qspi_read_cmd_format->address_lines == 2)
    {
        Cmdhandler.AddressMode = HAL_OSPI_ADDRESS_2_LINES;
    }else if(qspi_read_cmd_format->address_lines == 4)
    {
        Cmdhandler.AddressMode = HAL_OSPI_ADDRESS_4_LINES;
    }

    Cmdhandler.AlternateBytes = 0;
    Cmdhandler.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    Cmdhandler.AlternateBytesSize = 0;

    Cmdhandler.DummyCycles = qspi_read_cmd_format->dummy_cycles;

    Cmdhandler.NbData = read_size;
    if(qspi_read_cmd_format->data_lines == 0)
    {
        Cmdhandler.DataMode = HAL_OSPI_DATA_NONE;
    }else if(qspi_read_cmd_format->data_lines == 1)
    {
        Cmdhandler.DataMode = HAL_OSPI_DATA_1_LINE;
    }else if(qspi_read_cmd_format->data_lines == 2)
    {
        Cmdhandler.DataMode = HAL_OSPI_DATA_2_LINES;
    }else if(qspi_read_cmd_format->data_lines == 4)
    {
        Cmdhandler.DataMode = HAL_OSPI_DATA_4_LINES;
    }
   Cmdhandler.DataDtrMode             = HAL_OSPI_DATA_DTR_DISABLE;              //
   Cmdhandler.NbData                  = read_size;                          //读取长度

   Cmdhandler.DummyCycles             = 6;                                      // ¿ÕÖÜÆÚ¸öÊý
   Cmdhandler.DQSMode                 = HAL_OSPI_DQS_DISABLE;                   // ²»Ê¹ÓÃDQS 
    Cmdhandler.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;
    Cmdhandler.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;

	if (HAL_OSPI_Command(hospi, &Cmdhandler, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		log_e("sfud qspi commend write error!");
	} 
    if (HAL_OSPI_Receive(hospi, read_buf, 5000) != HAL_OK)
    {
        sfud_log_info("qspi recv data failed(%d)!", hospi->ErrorCode);
        hospi->State = HAL_OSPI_STATE_READY;
        result = SFUD_ERR_READ;
    }
    return result;
}
#endif /* SFUD_USING_QSPI */
void cs_on_w25q_spi(void)
{
		HAL_GPIO_WritePin(W25Q_NSS_GPIO_Port,W25Q_NSS_Pin,GPIO_PIN_RESET);
}
void cs_off_w25q_spi(void)
{
		HAL_GPIO_WritePin(W25Q_NSS_GPIO_Port,W25Q_NSS_Pin,GPIO_PIN_SET);
}


void ll_spi_tx(struct com_t *com)
{
#ifdef STM32H7
	SCB_CleanDCache_by_Addr((uint32_t *)com->task_1.pdata, (int32_t)com->task_1.len);
#endif
	//	if(com->task_1.mode == COM_UART_QUEUE_TX_STAGE1)log_d("\t\t\t\tCOM_UART_QUEUE_TX_STAGE1");
	//	if(com->task_1.mode == COM_UART_QUEUE_TX_STAGE2)log_d("\t\t\t\tCOM_UART_QUEUE_TX_STAGE2");
	HAL_SPI_Transmit(com->userdata, com->task_1.pdata, (int16_t)com->task_1.len,1000);
		com_spi_tx_cplt(&com_spi);

}
void ll_spi_rx(struct com_t *com)
{
//#ifdef STM32H7
//	SCB_CleanDCache_by_Addr((uint32_t *)com->task_1.pdata, (int32_t)com->task_1.len);
//#endif
	//	if(com->task_1.mode == COM_UART_QUEUE_TX_STAGE1)log_d("\t\t\t\tCOM_UART_QUEUE_TX_STAGE1");
	//	if(com->task_1.mode == COM_UART_QUEUE_TX_STAGE2)log_d("\t\t\t\tCOM_UART_QUEUE_TX_STAGE2");
	HAL_SPI_Receive(com->userdata, com->task_1.pdata, (int32_t)com->task_1.len,1000);
	com_spi_rx_cplt(&com_spi,com_spi.task_1.len);

}



sfud_err sfud_spi_port_init(sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your port spi bus and device object initialize code like this:
     * 1. rcc initialize
     * 2. gpio initialize
     * 3. spi device initialize
     * 4. flash->spi and flash->retry item initialize
     *    flash->spi.wr = spi_write_read; //Required
     *    flash->spi.qspi_read = qspi_read; //Required when QSPI mode enable
     *    flash->spi.lock = spi_lock;
     *    flash->spi.unlock = spi_unlock;
     *    flash->spi.user_data = &spix;
     *    flash->retry.delay = null;
     *    flash->retry.times = 10000; //Required
     */
	
	
	com_init_t init;
	init.rx_buf_size = 4096*2;
	init.tx_buf_size = 4096*2;
	init.rx_task_size = 2048;
	init.tx_task_size = 1024*8;
	com_init(&com_spi, com_type_spi, &init);
	com_driver_t driver;
	driver.user_rx_driver = ll_spi_rx;
	driver.user_tx_driver = ll_spi_tx;
	driver.user_data = &hspi6;//，目前无用
	com_register_driver(&com_spi,&driver);
	//com_rx_start(&com_spi, com_spi.rx_queue.datefifo->size);
	
	
	
	static sfud_userdata_t user_w25q64_spi = {
	.hspi=&hspi6,
	.cs_on=cs_on_w25q_spi,
	.cs_off=cs_off_w25q_spi,
	};
	static sfud_userdata_t user_w25q64_qspi = {
	.hospi=&hospi1,
	.cs_on=cs_on_w25q_spi,
	.cs_off=cs_off_w25q_spi,
	};
		
	switch(flash->index)
	{
		case SFUD_W25Q64JV_SPI_DEVICE_INDEX:
		 flash->spi.wr = spi_write_read; //Required
     flash->spi.lock = spi_lock;
     flash->spi.unlock = spi_unlock;
     flash->spi.user_data = &user_w25q64_spi;
     flash->retry.delay = &delay_1ms;
     flash->retry.times = 10000; //Required
			break;
		case SFUD_W25Q64JV_QSPI_DEVICE_INDEX:
		 flash->spi.wr = spi_write_read_qspi; //Required
     flash->spi.lock = spi_lock;
     flash->spi.unlock = spi_unlock;
		 flash->spi.qspi_read = qspi_read; //Required when QSPI mode enable
     flash->spi.user_data = &user_w25q64_qspi;
     flash->retry.delay = 0;
     flash->retry.times = 100000; //Required
			break;
		default:
			break;
	}

    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    //log_d("[SFUD](%s:%ld) ", file, line);
    /* must use vlog_d to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    log_d("%s\n", log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    //log_i("[SFUD]");
    /* must use vlog_d to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    log_i("%s\n", log_buf);
    va_end(args);
}


#define SFUD_DEMO_TEST_BUFFER_SIZE                     1024

static void sfud_demo(uint32_t addr, size_t size, uint8_t *data);

static uint8_t sfud_demo_test_buf[SFUD_DEMO_TEST_BUFFER_SIZE];

/**
 * SFUD demo for the first flash device test.
 *
 * @param addr flash start address
 * @param size test flash size
 * @param size test flash data buffer
 */
static void sfud_demo(uint32_t addr, size_t size, uint8_t *data) {
    sfud_err result = SFUD_SUCCESS;
    const sfud_flash *flash = sfud_get_device_table() + 0;
    size_t i;
    /* prepare write data */
    for (i = 0; i < size; i++) {
        data[i] = i;
    }
    /* erase test */
    result = sfud_erase(flash, addr, size);
    if (result == SFUD_SUCCESS) {
        printf("Erase the %s flash data finish. Start from 0x%08X, size is %d.\r\n", flash->name, addr,
                size);
    } else {
        printf("Erase the %s flash data failed.\r\n", flash->name);
        return;
    }
    /* write test */
    result = sfud_write(flash, addr, size, data);
    if (result == SFUD_SUCCESS) {
        printf("Write the %s flash data finish. Start from 0x%08X, size is %d.\r\n", flash->name, addr,
                size);
    } else {
        printf("Write the %s flash data failed.\r\n", flash->name);
        return;
    }
    /* read test */
    result = sfud_read(flash, addr, size, data);
    if (result == SFUD_SUCCESS) {
        printf("Read the %s flash data success. Start from 0x%08X, size is %d. The data is:\r\n", flash->name, addr,
                size);
        printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
        for (i = 0; i < size; i++) {
            if (i % 16 == 0) {
                printf("[%08X] ", addr + i);
            }
            printf("%02X ", data[i]);
            if (((i + 1) % 16 == 0) || i == size - 1) {
                printf("\r\n");
            }
        }
        printf("\r\n");
    } else {
          printf("Read the %s flash data failed.\r\n", flash->name);
    }
    /* data check */
    for (i = 0; i < size; i++) {
        if (data[i] != i % 256) {
            printf("Read and check write data has an error. Write the %s flash data failed.\r\n", flash->name);
			break;
        }
    }
    if (i == size) {
        printf("The %s flash test is success.\r\n", flash->name);
    }
}

void sfud_test(uint32_t addr)
{
	if(addr>= 0x00800000-sizeof(sfud_demo_test_buf))addr=0x00800000-sizeof(sfud_demo_test_buf);
	sfud_demo(addr, sizeof(sfud_demo_test_buf), sfud_demo_test_buf);
}

