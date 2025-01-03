/*
 * I2C.h
 *
 * Created: 8/15/2023 3:22:03 PM
 *  Author: osegh
 */ 


#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>

#define SDA 4
#define SCL 5

enum{
	I2C_ADDR_NACK = 0,
	I2C_OK = 1,
	I2C_DATA_NACK = 2,
	I2C_BUS_IDLE = 3,
};

void I2C_init();

void I2C_Start();
uint8_t I2C_Address(uint8_t addr, uint8_t rw);
uint8_t I2C_Write(uint8_t data);
uint8_t I2C_STATUS();
void I2C_Stop();

#endif /* I2C_H_ */