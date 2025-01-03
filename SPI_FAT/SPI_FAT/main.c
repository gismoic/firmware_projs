/*
 * main.c
 *
 * Created: 6/17/2023 5:44:55 PM
 *  Author: osegh
 */ 

#include <xc.h>
#define F_CPU 16000000UL
#include "MiSD.h"
#include "_74595.h"
#include "_74165.h"
#include "USART.h"
#include "SD.h"

int main(void)
{
	uint8_t og = 1;
	DDRC |= (1 << 0) | (1 << 1);
	DDRD |= (1 << 4);
	
	if(og == 1)
	{
		SPI_Init();
		SPCR |= (1 << SPR1);
		USART_init();
		
		uint8_t buffer[514];
		
		uint8_t init = MSD_SPI_Init();
		
		if(init == INIT_SUCCESS)
		{
			PORTC |= (1 << 0);
		}
		else
		{
			uint8_t init1 = MSD_SPI_Init();
			if(init1 != INIT_SUCCESS) PORTC |= (1 << 1);
		}
		
		if(init == INIT_SUCCESS)
		{
			_delay_ms(300);
			
			PORTC &= ~(1 << 0);
			PORTC &= ~(1 << 1);
			
			uint8_t data_res = MSD_SPI_READ_SINGLE_BLOCK(0x00000000, buffer);
			
			
			if(data_res == READ_BLOCK_SUCCESS)
			{
				PORTC |= (1 << 0);
			}
			else
			{
				PORTC |= (1 << 1);
				data_res = MSD_SPI_READ_SINGLE_BLOCK(0x00000000, buffer);
			}
			
			PORTC &= ~(1 << 0);
			PORTC &= ~(1 << 1);
			
			if(data_res == READ_BLOCK_SUCCESS)
			{
				PORTC |= (1 << 0);
			}
			else
			{
				PORTC |= (1 << 1);
			}
		}
	}
	if (og == 0)
	{
		uint8_t success = mmc_disk_initialize();
		
		if(success == 0)
		{
			PORTC |= (1 << 0);
		}
		else if(success != 0)
		{
			PORTC |= (1 << 1);
		}
	}
	
	
    while(1)
    {
		PORTD |= (1 << 4);
    }
}