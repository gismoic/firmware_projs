/*
 * _74HC164.h
 *
 * Created: 8/13/2023 8:51:16 PM
 *  Author: osegh
 */ 


#ifndef _74HC164_H_
#define _74HC164_H_

#include <avr/io.h>

#define SHLD 6
#define DAT 7
#define CLK 5

void _74HC165_init();
uint16_t _74HC165_read();

#endif /* _74HC164_H_ */