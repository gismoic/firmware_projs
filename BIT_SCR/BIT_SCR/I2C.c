/*
 * I2C.c
 *
 * Created: 8/15/2023 4:47:41 PM
 *  Author: osegh
 */

#include "I2C.h"

void I2C_init()
{
	TWCR |= (1 << TWEN);				               //enables TWI hardware
	
	DDRC |= (1 << SDA) | (1 << SCL);
	PORTC |= (1 << SDA) | (1 << SCL);				   //enable internal pullups for bus function (works for our purposes)
	
	TWSR |= (1 << TWPS0);
	TWSR &= ~(1 << TWPS1);				               //prescaler bits value: 4
	TWBR = 2;										   //calculated bit rate value, this should be correct idk tho (according to my math I should be right) OG:6
	TWCR |= (1 << TWIE);
}

void I2C_Start()
{
	TWCR |= (1 << TWSTA) | (1 << TWINT);
}

void I2C_Stop()
{
	while(!(TWCR & (1 << TWINT)));
	TWCR |= (1 << TWSTO); //stop sequence sent, end of I2C communication
	TWCR |= (1 << TWINT);
	while(TWCR & (1 << TWSTO)); // wait for stop to finish
}

uint8_t I2C_Address(uint8_t addr, uint8_t rw)
{
	while(!(TWCR & (1 << TWINT))); //wait till TWI finishes current task
	if((TWSR & 0xF8) == 0x08){ //start sequence was sent properly
		TWDR = (addr << 1) | rw; //I2C addresses are 7-bits wide with the 8th bit representing either a read or write operation 1: read 0: write
		TWCR &= ~(1 << TWSTA);
		TWCR |= (1 << TWINT);  //TWI hardware moves on to next scheduled task. TWINT is set low when a task is complete set high when it is set to move on to another task.
	}
	
	if((TWSR & 0xF8) == 0x20){
		return I2C_ADDR_NACK;
	}
	return I2C_OK;
}

uint8_t I2C_Write(uint8_t data)
{
	while(!(TWCR & (1 << TWINT)));
	if(((TWSR & 0xF8) == 0x18) || ((TWSR & 0xF8) == 0x28)){ //SLA + W/Data sequence was sent properly with ACK
		TWDR = data;
		TWCR &= ~(1 << TWSTA);
		TWCR |= (1 << TWINT);
	
	}else if((TWSR & 0xF8) == 0x30)
	{
		return I2C_DATA_NACK;
	}
	return I2C_OK;
}

uint8_t I2C_STATUS()
{
	return ((TWSR & 0xF8));
}
