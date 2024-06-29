#ifndef __USART_H
#define __USART_H

#include "at32f403a_407.h"

#define USART_RECEVIE_LEN 32
static uint8_t uart_fail[1] = {0x00};
static uint8_t uart_success[1] = {0x01};
struct Receiver_Struct {
  uint8_t rxbuf[USART_RECEVIE_LEN];
  uint8_t state;
  uint16_t len;
};

extern struct Receiver_Struct Receiver;

void usartdmasend(uint8_t *data, uint16_t len);
void usartdmarecv(uint8_t *data, uint16_t len);

void usart1_dma_init(uint32_t bound);

#endif
