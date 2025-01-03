/*
 * SSD1306.h
 *
 * Created: 8/17/2023 5:10:35 PM
 *  Author: osegh
 */ 

/*

	the software used in this program is designed to work with SSD1306 driven screens
	with dimensions 128x32 pixels. Compatibility is not ensured with screens with similar tech
	but are larger than 128x32 pixels. Modifications will need to be preformed by the user to adjust to 
	new hardware properly. As in, changing display lines, COM configurations, Buffer size, and more.
	
	See datasheet for more details regarding SSD1306 architecture.
	PS: this program is designed to work in serial mode so I2C MT functions are required
	
	- Ose 

*/


#ifndef SSD1306_H_
#define SSD1306_H_

#include <avr/io.h>
#include <stdlib.h>
#include "I2C.h"

void SSD1306_software_begin();
void SSD1306_display(uint8_t on_off); //1: display is on 0: display is off

void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t on_off);
void SSD1306_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t on_off);
void SSD1306_DrawLine(uint8_t x, uint8_t y, uint8_t xp, uint8_t yp);


void SSD1306_clearDisplay();
void SSD1306_display_GDDRAM();

#endif /* SSD1306_H_ */