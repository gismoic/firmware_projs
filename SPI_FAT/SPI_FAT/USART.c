/*
 * USART.c
 *
 * Created: 7/7/2023 5:06:31 PM
 *  Author: osegh
 */ 
#include "USART.h"

void USART_init()
{
	 UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);	 UCSR0B |= (1 <<  TXEN0);
	 UBRR0L = BAUD;
	 UBRR0H = BAUD >> 8;
}

void USART_transmit(uint8_t data)
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void USART_transmit_word(uint8_t word[])
{
	uint8_t i = 0;
	
	if(!(word[i] == 0))
	{
		i++;
		USART_transmit(word[i]);
	}
}

void USART_puthex8(uint8_t val)
{
	uint8_t upperNibble = (val & 0xF0) >> 4;
	uint8_t lowerNibble = val & 0x0F;

	upperNibble += upperNibble > 9 ? 'A' - 10 : '0';
	lowerNibble += lowerNibble > 9 ? 'A' - 10 : '0';
	
	USART_transmit('0');
	USART_transmit('X');
	USART_transmit(upperNibble);
	USART_transmit(lowerNibble);
}