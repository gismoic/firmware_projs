/*
 * MiSD.c
 *
 * Created: 6/17/2023 6:26:42 PM
 *  Author: osegh
 */ 
#include "MiSD.h"


void MSD_Transmition(uint8_t CMD, uint32_t ARG, uint8_t CRC)
{
	SPI_Transmit(CMD | 0x40);
	
	SPI_Transmit((uint8_t)(ARG >> 24));
	SPI_Transmit((uint8_t)(ARG >> 16));
	SPI_Transmit((uint8_t)(ARG >> 8));
	SPI_Transmit((uint8_t)(ARG));
	
	SPI_Transmit(CRC | 0x01);
}

uint8_t readRes_1()
{
	uint8_t i = 0, res1;
	
	while((res1 = SPI_Transmit(0xFF)) == 0xFF)
	{
		i++;
		
		if(i > 8) return 0xFF; //this means there's an error check hardware connections
	}
	
	return res1; //something that isn't 0xFF
}

void readRes_7(uint8_t res[])
{
	res[0] = readRes_1();
	
	if(res[0] > 1) return;
	
	res[1] = SPI_Transmit(0xFF);
	res[2] = SPI_Transmit(0xFF);
	res[3] = SPI_Transmit(0xFF);
	res[4] = SPI_Transmit(0xFF);
	
}

void readRes_3(uint8_t *res[])
{
	res[0] = readRes_1();
	if(res[0] > 1) return;
	
	res[1] = SPI_Transmit(0xFF);
	res[2] = SPI_Transmit(0xFF);
	res[3] = SPI_Transmit(0xFF);
}

//more like a power up sequence pulse the SCK line at least 75 times
void SPI_warmup()
{
	PORTD &= ~(1 << CS);
	
	_delay_ms(1);
		
	for(uint8_t i = 0; i < 10; i++)
	{
		SPI_Transmit(0xFF);
	}
	
	PORTD |= (1 << CS);
}

uint8_t SPI_IDLE()
{
	//designed to assert the Micro SD card properly
	SPI_Transmit(0xFF);
	PORTB &= ~(1 << CS);
	SPI_Transmit(0xFF);
	
	MSD_Transmition(0, 0, 0x94);
	uint8_t idle_res = readRes_1();
	
	SPI_Transmit(0xFF);
	PORTB |= (1 << CS);
	SPI_Transmit(0xFF);
	
	return idle_res;
}

void SPI_READ_OCR(uint8_t *res[])
{
	SPI_Transmit(0xFF);
	PORTB &= ~(1 << CS);
	SPI_Transmit(0xFF);
	
	MSD_Transmition(58, 0x0000000, 0);
	readRes_3(res);
	
	SPI_Transmit(0xFF);
	PORTB |= (1 << CS);
	SPI_Transmit(0xFF);
	
}

void SPI_IF_CON(uint8_t res[])
{
	//designed to assert the Micro SD card properly
	SPI_Transmit(0xFF);
	PORTB &= ~(1 << CS);
	SPI_Transmit(0xFF);
	
	MSD_Transmition(8, 0x000001AA, 0x87); 
	readRes_7(res);
	
	SPI_Transmit(0xFF);
	PORTB |= (1 << CS);
	SPI_Transmit(0xFF);
}

uint8_t SPI_APP_CMD()
{
	SPI_Transmit(0xFF);
	PORTB &= ~(1 << CS);
	SPI_Transmit(0xFF);
	
	MSD_Transmition(55, 0, 0);
	uint8_t res = readRes_1();
	
	SPI_Transmit(0xFF);
	PORTB |= (1 << CS);
	SPI_Transmit(0xFF);
	
	return res;
}

uint8_t SPI_SEND_OP()
{
	SPI_Transmit(0xFF);
	PORTB &= ~(1 << CS);
	SPI_Transmit(0xFF);
	
	MSD_Transmition(41, 0x40000000, 0); 
	uint8_t res = readRes_1();
	
	SPI_Transmit(0xFF);
	PORTB |= (1 << CS);
	SPI_Transmit(0xFF);
	
	return res;
}

uint8_t MSD_SPI_Init()
{
	SPI_warmup();
	uint8_t res1 = SPI_IDLE();
	
	if(res1 != 0x01)
	{
		return INIT_FAILURE;
	}
	
	uint8_t echo[4];
	SPI_IF_CON(echo);
	
	//paranoia
	
	if((echo[4] != 0xAA) && (echo[3] != 0x01))
	{
		return INIT_FAILURE;
	}
	
	
	uint8_t OCR[5];
	SPI_READ_OCR(OCR);
	
	uint16_t i = 0;
	uint8_t exRes[4];
	do 
	{
		if(i > 100)
		{
			return INIT_FAILURE; 
		}
		
		exRes[0] = SPI_APP_CMD();
		if(exRes[0] < 2)
		{
			exRes[0] = SPI_SEND_OP();
		}
		i++;
		
	} while (exRes[0] != 0x00);
	
	SPI_READ_OCR(OCR);
	return INIT_SUCCESS;
}

uint8_t MSD_SPI_READ_SINGLE_BLOCK(uint32_t addr, uint8_t *data[])
{
	token_t _token = {
		
		.start_block = 0xFE,
		.timeout = 0xFF,
	};
	
	SPI_Transmit(0xFF);
	PORTB &= ~(1 << CS);
	SPI_Transmit(0xFF);
	
	uint8_t res = 0;
	uint8_t try = 0;
	
	do 
	{
		if(try >= 255) return READ_BLOCK_FAILURE;
		
		MSD_Transmition(17, addr, 0);
		uint8_t res = readRes_1();
		try++;
	} while (res == 0x00);
	
	if(res != 0xFF)
	{
		uint16_t token_read_attempts = 0;
		uint8_t token =  0;
		
		do 
		{
			if(token_read_attempts == 500)
			{
				return READ_BLOCK_FAILURE;
			}
			
			token_read_attempts++;
			
		} while ((token = SPI_Transmit(0xFF) != _token.start_block));
		
		
		for(int i = 0; i < 514; i++)
		{
			*data[i] = SPI_Transmit(0xFF);
		}
	}
	
	SPI_Transmit(0xFF);
	PORTB |= (1 << CS);
	SPI_Transmit(0xFF);
	
	return READ_BLOCK_SUCCESS;
}