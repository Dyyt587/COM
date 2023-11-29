#ifndef COM_COMPONENT_UART_H
#define COM_COMPONENT_UART_H


#include "com_component.h"
#include "com_component_def.h"





uint8_t com_uart_tx_cplt(com_t *com);
uint8_t com_uart_tx(com_t *com, uint32_t addr, uint8_t *pdata, uint32_t length, uint32_t delaytime);

void com_uart_init(com_t *com, uint32_t buf_size);

void com_user_init(void);

void com_uart_link(com_t *com, void (*ll_tx)(struct com_t *com));
void com_uart_linkrx(com_t *com, void (*ll_rx)(struct com_t *com));

uint8_t com_uart_rx_pack_cplt(com_t *com, uint32_t len);
uint8_t com_uart_rx_start(com_t *com, uint32_t len);
uint8_t com_uart_rx(struct com_t *com, uint32_t addr, uint8_t *pdata, uint32_t length, uint32_t delaytime);


void com_rx_init(com_t *com, uint32_t size);
uint32_t com_rx_uart_get(com_t *com, uint32_t *padta,uint32_t len);
#endif
