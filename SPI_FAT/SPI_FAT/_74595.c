/*
 * _74595.c
 *
 * Created: 6/26/2023 1:33:10 PM
 *  Author: osegh
 */

#include "_74595.h"

void Init_shift()
{
	DDRD |= (1 << SER) | (1 << RCLK);
	DDRD |= (1 << SRCLK);
} 

void pulse_clk()
{
	PORTD |= (1 << SRCLK);
	PORTD &= ~(1 << SRCLK);
}

void shift_write(uint8_t data)
{
	PORTD &= ~(1 << RCLK);
	
	for(uint8_t i = 0; i < 8; i++)
	{
		if(data & 0b10000000)
		{
			PORTD |= (1 << SER);
		}
		else
		{
			PORTD &= ~(1 << SER);
		}
		
		pulse_clk();
		data = data << 1;
	}
	
	PORTD |= (1 << RCLK);
}
