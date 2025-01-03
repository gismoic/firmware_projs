/*
 * _74HC164.c
 *
 * Created: 8/13/2023 8:58:18 PM
 *  Author: osegh
 */ 
#include "_74HC164.h"

void _74HC165_init()
{
	DDRD |= (1 << SHLD) | (1 << CLK);
	DDRD &= ~(1 << DAT);
}

void pulse_clk()
{
	PORTD |= (1 << CLK);
	PORTD &= ~(1 << CLK);
}

uint16_t _74HC165_read()
{
	uint16_t tmp = 0;
	PORTD &= ~(1 << SHLD);
	PORTD |= (1 << SHLD);
	for (uint16_t i = 0; i < 16; i++)
	{
		if(PIND & (1 << DAT))
		{
			tmp |= (1 << i);	
		}
		else
		{
			tmp &= ~(1 << i);	
		}
		pulse_clk();
	}
	return tmp;
}