/*
 * USART.h
 *
 * Created: 7/7/2023 5:01:23 PM
 *  Author: osegh
 */ 


#ifndef USART_H_
#define USART_H_

#include <avr/io.h>

#define BAUD  ((16000000/16/9600) - 1)

void USART_init();
void USART_transmit(uint8_t data);
void USART_transmit_word(uint8_t word[]);
void USART_puthex8(uint8_t val);

#endif /* USART_H_ */