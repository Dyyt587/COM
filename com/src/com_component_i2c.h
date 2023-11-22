#ifndef COM_COMPONENT_I2C_H
#define COM_COMPONENT_I2C_H


#include "com_component_def.h"

// 对于uart的tx和rx 由两个queue组成
// 对于iic, 由一个queue组成
// 对于spi 由两个queue组成,但是taskfifo是同一个，进行特别初始化



void com_i2c_init(com_t *com);
uint8_t com_i2c_tx_cplt(com_t *com);
uint8_t com_i2c_tx(com_t *com, uint32_t addr, uint8_t *pdata, uint32_t length, uint32_t delaytime);

 //   uint8_t (*com_rx)(struct com_t *com, uint32_t addr, uint8_t *pdata, uint32_t length, uint32_t delaytime);

uint8_t com_i2c_start_rx(com_t *com, uint32_t addr, uint8_t *pdata, uint32_t length, uint32_t delaytime);

uint8_t com_i2c_rx_cplt(com_t *com,uint32_t len);

#endif
