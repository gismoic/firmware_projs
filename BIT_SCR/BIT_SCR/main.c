/*
 * BIT_SCR.c
 *
 * Created: 8/13/2023 8:50:11 PM
 * Author : osegh
 */ 

#include "main.h"

int main(void)
{
    
	_74HC165_init();
	I2C_init();
	
	SSD1306_software_begin();
	_delay_ms(50);
	DDRC |= (1 << 0) | (1 << 1);
	
    while (1) 
    {
		SSD1306_DrawRect(0, 0, 127, 31, 0);
		
		for(uint8_t i = 1; i < 6; i++)
		{
			_delay_ms(80);
			SSD1306_DrawRect(0 + (3*i), 0 + (3*i), 127 - (6*i), 31 - (5*i), 0);
			
			if(i == 5)
			{
				SSD1306_DrawRect(0 + (3*i), 0 + (3*i), 127 - (6*i), 31 - (5*i), 1);
			}
			
			_delay_ms(100);
			
			SSD1306_display_GDDRAM();
		}
		
		_delay_ms(100);
		SSD1306_clearDisplay();
		SSD1306_display_GDDRAM();
		
    }
}

