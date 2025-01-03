/*
 * SSD1306.c
 *
 * Created: 8/17/2023 5:10:51 PM
 *  Author: osegh
 */ 

#include "SSD1306.h"
#include <math.h>

uint8_t GDDRAM_BUFFER[512]; //buffer used to store graphics data before it's written to GDDRAM
#define I2C_ADDRESS 0x3C    //SSD1306 I2C address decided by a unique pin config on the chip

#define SCREEN_WIDTH 128    //measured in pixels on display
#define SCREEN_HEIGHT 32    //measured in pixels on display


//screen control commands
#define DISPLAY_ON				0xAF
#define DISPLAY_OFF				0xAE

#define DISPLAY_RAM				0xA4
#define ENTIRE_DISPLAY_ON		0xA5 //this mode disables RAM display

#define MUX_RATIO				0xA8
#define DISPLAY_OFFSET			0xD3
#define HARDWARE_CONFIG			0xDA
#define CONTRAST_CONTROL		0x81
#define INVERSE_OPERATION		0xA7 //1 : pixel is off  0 : pixel is on
#define NORMAL_OPERATION		0xA6 //1 : pixel is on   0 : pixel is off
#define OPCLK_DIVIDER			0xD5
#define ENABLE_CHARGE_PUMP		0x8D

void SEND_COMMAND(uint8_t bits, uint8_t data)
{
	I2C_Write(bits << 6); //Command byte
	I2C_Write(data);	  //Data byte
}

void SSD1306_software_begin()
{
	I2C_Start();
	I2C_Address(I2C_ADDRESS, 0);
	
	SEND_COMMAND(0b00, DISPLAY_OFF);
	SEND_COMMAND(0b00, MUX_RATIO);
	I2C_Write(0xFF); //sets MUX ratio to 32 -or more- to fit within screen physical boundaries
	SEND_COMMAND(0b00, DISPLAY_OFFSET);
	I2C_Write(0x00); //zero display offset
	SEND_COMMAND(0b00, 0x40); //display line is set to COM0
	SEND_COMMAND(0b00, 0xA0); //default segment configuration
	SEND_COMMAND(0b00, 0xC0); //display scanning direction (normal COM0 - COM32)
	SEND_COMMAND(0b00, HARDWARE_CONFIG);
	I2C_Write(0x02); //standard hardware config
	SEND_COMMAND(0b00, CONTRAST_CONTROL);
	I2C_Write(0x7F); //half contrast, can be changed for power consumption reasons
	SEND_COMMAND(0b00, DISPLAY_RAM); //used to turn on entire screen
	SEND_COMMAND(0b00, NORMAL_OPERATION);
	SEND_COMMAND(0b00, OPCLK_DIVIDER);
	I2C_Write(0x80); //"D" value to calculate internal RC Oscillator frequency output
	SEND_COMMAND(0b00, ENABLE_CHARGE_PUMP);
	I2C_Write(0x14); //enables charge pump when on
	SEND_COMMAND(0b00, DISPLAY_ON);
	
	I2C_Stop();
}

void SSD1306_display(uint8_t on_off)
{
	I2C_Start();
	I2C_Address(0x3C, 0);
	on_off ?  SEND_COMMAND(0b00, DISPLAY_ON) : SEND_COMMAND(0b00, DISPLAY_OFF);
	I2C_Stop();
}

void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t on_off)
{
	switch (on_off)
	{
		case 0:
			GDDRAM_BUFFER[x + (y/8) * SCREEN_WIDTH] &= ~(1 << (y & 7)); //stolen from Adafruit_SSD1306.cpp (couldn't figure it out -\_('_')_/-)
			break;
		case 1:
			GDDRAM_BUFFER[x + (y/8) * SCREEN_WIDTH] |= (1 << (y & 7));
			break;
	}
}

void SSD1306_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t on_off)
{
	for(uint8_t width = 0; width < w; width++)
	{
		SSD1306_DrawPixel(x + width, y, 1);
	}
	for(uint8_t width = 0; width < w; width++)
	{
		SSD1306_DrawPixel(x + width, y + h, 1);
	}
	for(uint8_t height = 0; height < h; height++)
	{
		SSD1306_DrawPixel(x + w, y + height, 1);
	}
	for(uint8_t height = 0; height < h; height++)
	{
		SSD1306_DrawPixel(x, y + height, 1);
	}
	
	for(uint8_t height = 1; height < h; height++)
	{
		for(uint8_t width = 1; width < w; width++)
		{
			SSD1306_DrawPixel(x + width, y + height, on_off);
		}
	}
}

void SSD1306_DrawLine(uint8_t x, uint8_t y, uint8_t xp, uint8_t yp)
{
	uint8_t distx = xp - x;
	uint8_t disty = yp - y;
	
	uint16_t hyp = (uint8_t)sqrt((double)((distx^2) + (disty^2)));
	
	uint8_t y_slope = yp - y;
	uint8_t x_slope = xp - x;
	uint8_t slope = y_slope/x_slope;
	
	for(uint8_t i = 0; i < hyp; i++)
	{
		for (uint8_t _y = 0; _y < slope; y++)
		{
			SSD1306_DrawPixel(x + i, y + _y, 1);
		}
	}
	
}


void SSD1306_clearDisplay()
{
	for(uint16_t i = 0; i < 512; i++)
	{
		GDDRAM_BUFFER[i] = 0x00;
	}
}

void SSD1306_display_GDDRAM()
{
	I2C_Start();
	I2C_Address(I2C_ADDRESS, 0);
	
	SEND_COMMAND(0b00, 0x20);	   //addressing mode change command
	I2C_Write(0x00);			   //horizontal addressing mode select
	
	SEND_COMMAND(0b00, 0x21);      //setting column address
	I2C_Write(0);			       //starting column address
	I2C_Write(0xFF);		       //ending column address
	
	SEND_COMMAND(0b00, 0x22);      //setting page address
	I2C_Write(0x00);			   //starting page address
	I2C_Write(0xFF);			   //ending page address
	
	I2C_Stop();
	
	I2C_Start();
	I2C_Address(I2C_ADDRESS, 0);
	SEND_COMMAND(0x00, 0x40);
	I2C_Stop();
	
	for(uint8_t y = 0; y < SCREEN_HEIGHT; y++){
		I2C_Start();
		I2C_Address(I2C_ADDRESS, 0);
		I2C_Write(0x40);
		for(uint8_t x = 0; x < SCREEN_WIDTH/8; x++){
			I2C_Write(GDDRAM_BUFFER[x + (16*y)]);
		}
		
		I2C_Stop();
	}
}

