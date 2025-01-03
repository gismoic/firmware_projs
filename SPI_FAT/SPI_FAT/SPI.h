/*
 * SPI.h
 *
 * Created: 6/17/2023 5:46:08 PM
 *  Author: osegh
 */ 


#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define MISO PORTB4
#define MOSI PORTB3
#define SCK  PORTB5
#define CS	 PORTB2

void SPI_Init();
uint8_t SPI_Transmit(uint8_t data);
void SPI_disable();

#endif /* SPI_H_ */