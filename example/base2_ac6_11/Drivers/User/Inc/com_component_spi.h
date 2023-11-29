#ifndef COM_COMPONENT_SPI_H
#define COM_COMPONENT_SPI_H

#include "com_component.h"
#include "com_component_def.h"

uint8_t com_spi_tx_cplt(com_t *com);
uint8_t com_spi_rx_cplt(com_t *com,uint32_t len);
uint8_t com_spi_tx(com_t *com, uint32_t addr, uint8_t *pdata_tx, uint32_t length, uint32_t delaytime);
uint8_t com_spi_start_rx(com_t *com, uint32_t addr, uint32_t length, uint32_t delaytime);
#endif
