/*
 * LED_BLINK_555.c
 *
 * Created: 6/21/2023 7:36:48 PM
 * Author : osegh
 */ 

#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>

int main(void)
{
	DDRB |= (1 << 4);
	
    /* Replace with your application code */
    while (1) 
    {
		PORTB |= (1 << 1);
		_delay_ms(10);
		PORTB &= ~(1 << 1);
    }
}

