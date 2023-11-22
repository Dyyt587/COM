#include "sht30.h"
#include "i2c.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "usart.h"
#include "com_component.h"
#include "elog.h"

uint8_t flag_tx=0;
uint8_t flag_rx=0;

com_t com_i2c1;

void ll_iic_tx (com_t* com)
{
//	for(int i=0;i<com->buflength;++i){
//			log_d("%d", *((com->buf)+i));
//	}4
	if(com->task_1.mode == COM_I2C_QUEUE_TX_STAGE1 )
	{
		HAL_I2C_Master_Seq_Transmit_DMA(&hi2c3, SHT30_ADDR_WR, (uint8_t*)com->task_1.pdata, com->task_1.len, I2C_FIRST_FRAME);
	}else if(com->task_1.mode == COM_I2C_QUEUE_TX_STAGE2){
		HAL_I2C_Master_Seq_Transmit_DMA(&hi2c3, SHT30_ADDR_WR, (uint8_t*)com->task_1.pdata, com->task_1.len, I2C_LAST_FRAME);

	}else{
		HAL_I2C_Master_Seq_Transmit_DMA(&hi2c3, SHT30_ADDR_WR, (uint8_t*)com->task_1.pdata, com->task_1.len, I2C_FIRST_AND_LAST_FRAME);

	}
		
	//HAL_I2C_Master_Transmit_DMA(&hi2c3, SHT30_ADDR_WR, (uint8_t*)com->buf, com->buflength);
	
}

void ll_iic_rx (com_t* com)
{
//for(int i=0;i<com->buflength;++i){
//		log_d("%d", *((com->buf)+i));
//}
	if(com->task_1.mode == COM_I2C_QUEUE_RX_STAGE1 )
	{
		HAL_I2C_Master_Seq_Receive_DMA(&hi2c3, SHT30_ADDR_WR, (uint8_t*)com->task_1.pdata, com->task_1.len, I2C_FIRST_FRAME);
	}else if(com->task_1.mode == COM_I2C_QUEUE_RX_STAGE2){
		HAL_I2C_Master_Seq_Receive_DMA(&hi2c3, SHT30_ADDR_WR, (uint8_t*)com->task_1.pdata, com->task_1.len, I2C_LAST_FRAME);
	}else{
		HAL_I2C_Master_Seq_Receive_IT(&hi2c3, SHT30_ADDR_WR, (uint8_t*)com->task_1.pdata, com->task_1.len, I2C_FIRST_AND_LAST_FRAME);
	}
		
	//HAL_I2C_Master_Transmit_DMA(&hi2c3, SHT30_ADDR_WR, (uint8_t*)com->buf, com->buflength);
	
}

void sht30_init(void)
{
	com_i2c_init(&com_i2c1);
	com_i2c1.ll_tx = ll_iic_tx;
	com_i2c1.ll_rx = ll_iic_rx;
}

int report_tempRH_json(void)
{

	float     temperature, humidity;

	if(SHT30_SampleData(&temperature, &humidity) < 0)
	{
		log_e("ERROR: SHT30 Sample data failure");
		return -1;
	}

	log_i("{\"Temperature\":\"%.2f\", \"Humidity\":\"%.2f\"}\n", temperature, humidity);
	

	return 0;
}

/* 第1个参数为I2C操作句柄
   第2个参数为从机设备地址
   第3个参数为发送的数据的起始地址
   第4个参数为传输数据的大小
   第5个参数为操作超时时间 　　*/


static int sht30_send_cmd(SHT30_CMD cmd)
{
	uint8_t buf[2];

	buf[0] = cmd >> 8;
	buf[1] = cmd & 0xFF;
//	uint8_t rec = HAL_I2C_Master_Transmit(&hi2c3, SHT30_ADDR_WR, (uint8_t*)buf, 2, 0xFFFF);
	com_i2c_tx(&com_i2c1,SHT30_ADDR_WR,(uint8_t*)buf, 2,COM_DELAY_EVERY);	
	uint8_t rec=0;

	return rec;
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{

	com_i2c_tx_cplt(&com_i2c1);

}


void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	com_i2c_rx_cplt(&com_i2c1,0);
	flag_rx=0;    
}

static void sht30_soft_reset(void)
{
	sht30_send_cmd(SOFT_RESET_CMD);
	//HAL_Delay(1);
}


static int sht30_single_shot_measurement(uint8_t *buf, uint8_t buf_size)
{
	uint16_t cmd = HIGH_ENABLED_CMD;
	int  rv=0;

	if( !buf || buf_size < SHT30_DATA_SIZE )
	{
		log_e("%s(): Invalid input argument", __func__);
		return -1;
	}

	rv = sht30_send_cmd(cmd);
	if( rv )
	{
		log_e("ERROR: HST30 send messurement command failure, rv = %d", rv);
		sht30_soft_reset();
		return -2;
	}
	//rv = HAL_I2C_Master_Receive(&hi2c3, SHT30_ADDR_RD, buf, SHT30_DATA_SIZE, 0xFFFF);
	flag_rx=1;  
	com_i2c_start_rx(&com_i2c1,SHT30_ADDR_RD,buf,SHT30_DATA_SIZE,COM_DELAY_EVERY);	
	//rv = HAL_I2C_Master_Receive_IT(&hi2c3, SHT30_ADDR_RD, buf, SHT30_DATA_SIZE);
	while(flag_rx);
	kfifo_get(com_i2c1.rx_queue.datefifo,buf,SHT30_DATA_SIZE);

	if(rv)
	{
		log_e("ERROR: SHT30 read measurement result failure, rv = %d", rv);
		return -3;
	}

	return 0;
}

static uint8_t sht30_crc8(const uint8_t *data, int len)
{
	const uint8_t POLYNOMIAL = 0x31;
	uint8_t       crc = 0xFF;
	int           i,j;

	for (i=0; i<len; ++i)
	{
		crc ^= *data++;

		for (j=0; j<8; j++)
		{
			crc = (crc & 0x80)? (crc << 1) ^ POLYNOMIAL:(crc << 1);
		}
	}
	return crc;
}

int SHT30_SampleData(float *temperature, float *humidity)
{
	uint8_t buf[SHT30_DATA_SIZE];
	int rv;
	uint16_t temp;
	uint16_t humd;
	uint16_t crc;

	if(!temperature || !humidity)
	{
		log_e("%s(): Invalid input argument", __func__);
		return -1;
	}

	rv = sht30_single_shot_measurement(buf, SHT30_DATA_SIZE);
	if(rv)
	{
		log_e("SHT30 Single Short measurement failure, rv=%d", rv);
		return -2;
	}

#ifdef CONFIG_SHT30_DEBUG
	{
		int i;

		sht30_print("SHT30 get %d bytes sample data: \n", SHT30_DATA_SIZE);
		for(i=0; i<SHT30_DATA_SIZE; i++)
		{
			sht30_print("0x%02x ", buf[i]);
		}
		sht30_print("\n");
	}
#endif

	crc = sht30_crc8(buf, 2);
	log_i("SHT30 temperature Cal_CRC: [%02x] EXP_CRC: [%02x]", crc, buf[2]);
	if(crc != buf[2])
	{
		log_e("SHT30 measurement temperature got CRC error");
		return -3;
	}

	crc = sht30_crc8(&buf[3], 2);
	log_i("SHT30 humidity Cal_CRC: [%02x] EXP_CRC: [%02x]", crc, buf[5]);
	if(crc != buf[5])
	{
		log_e("SHT30 messurement temperature got CRC error");
		return -4;
	}

	temp = (buf[0]<<8) | buf[1];
	humd = (buf[3]<<8) | buf[4];

	*temperature = -45 + 175*((float)temp/65535);
	*humidity = 100 * ((float)humd/65535);

	return 0;
}

