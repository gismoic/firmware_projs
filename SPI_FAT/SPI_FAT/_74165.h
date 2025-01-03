/*
 * _74HC165.h
 *
 * Created: 7/2/2023 9:34:11 PM
 *  Author: osegh
 */ 


#ifndef _74165_H_
#define _74165_H_

#include <avr/io.h>

#define CLK 2
#define INH 3
#define OUT 4
#define LAT 5

void _165_init();
uint16_t _165_read();


#endif /* _74HC165_H_ */