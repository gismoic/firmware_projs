/*
 * _74595.h
 *
 * Created: 6/26/2023 1:31:56 PM
 *  Author: osegh
 */ 


#ifndef _74595_H_
#define _74595_H_

#include <avr/io.h>

#define SER 7
#define RCLK 6
#define SRCLK 5

void Init_shift();
void shift_write(uint8_t data);

#endif /* 74595_H_ */