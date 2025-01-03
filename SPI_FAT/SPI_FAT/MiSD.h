/*
 * MiSD.h
 *
 * Created: 6/17/2023 6:26:23 PM
 *  Author: osegh
 */ 


#ifndef MISD_H_
#define MISD_H_

#include <avr/io.h>
#include <util/delay.h>
#include "SPI.h"
#include "USART.h"

enum{
	
	INIT_FAILURE = 1,
	INIT_SUCCESS = 0,
	
	READ_BLOCK_FAILURE = 3,
	READ_BLOCK_SUCCESS = 2,
		
};

struct token_types{
	uint8_t start_block;
	uint8_t timeout;
};
typedef struct token_types token_t;

#define CS PORTB2


uint8_t MSD_SPI_Init();
uint8_t MSD_SPI_READ_SINGLE_BLOCK(uint32_t addr, uint8_t *data[]);

#endif /* MISD_H_ */