/*
 * _74165.c
 *
 * Created: 7/2/2023 9:45:22 PM
 *  Author: osegh
 */ 
#include "_74165.h"

void _165_init()
{
	DDRC |= (1 << CLK) | (1 << INH) | (1 << LAT);
	DDRC &= ~(1 << OUT);
	
	PORTC &= ~(1 << INH);
}

void pulse_clock()
{
	PORTC |= (1 << CLK);
	PORTC &= ~(1 << CLK);
}

uint16_t _165_read()
{
	uint16_t data = 0;
	PORTC &= ~(1 << LAT);
	PORTC |= (1 << LAT);
	
	for(uint16_t i = 0; i < 16; i++)
	{
		if(PINC & (1 << OUT))
		{
			data |= (1 << i);
		}
		else
		{
			data &= ~(1 << i);
		}
		
		pulse_clock();
	}
	
	return data; 
}