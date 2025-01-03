/*~
 * SPI.c
 *
 * Created: 6/17/2023 5:46:20 PM
 *  Author: osegh
 */ 
#include "SPI.h"

void SPI_Init()
{
	DDRB |=  (1 << CS) | (1 << MOSI) | (1 << SCK);
	DDRB |= (1 << MISO);

	SPCR = (1 << SPE) | (1 << MSTR);
}

uint8_t SPI_Transmit(uint8_t data)
{
	SPDR = data;
	while(!(SPSR & (1 << SPIF)));
	return SPDR;
}

void SPI_disable()
{
	//shuts down SPI hardware
	SPCR = 0;
}