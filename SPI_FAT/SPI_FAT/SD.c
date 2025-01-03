/*
 * SD.c
 *
 * Created: 7/13/2023 2:58:19 PM
 *  Author: osegh
 */ 
#include "SD.h"
#include <util/delay.h>

#define CS_LOW()		(PORTB &= ~(1 << PORTB2))	/* Set MMC_CS = low */
#define	CS_HIGH()		(PORTB |= (1 << PORTB2))	/* Set MMC_CS = high */
#define	FCLK_SLOW()		(SPCR |= (1 << SPR1))	/* Set SPI clock for initialization (100-400kHz) */
#define	FCLK_FAST()		(SPCR &= ~(1 << SPR1))	/* Set SPI clock for read/write (20MHz max) */

#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND (MMC) */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD32	(32)		/* ERASE_ER_BLK_START */
#define CMD33	(33)		/* ERASE_ER_BLK_END */
#define CMD38	(38)		/* ERASE */
#define	CMD48	(48)		/* READ_EXTR_SINGLE */
#define	CMD49	(49)		/* WRITE_EXTR_SINGLE */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */


static volatile uint8_t Stat = 0x01;	/* Disk status or not initialized*/

/* 100Hz decrement timers */
static volatile uint8_t Timer1;
static volatile unsigned int Timer2;

static uint8_t CardType;			/* Card type flags (b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing) */


static void power_on (void)
{
	SPI_Init();
}


static void power_off (void)
{
	SPI_disable();
}

/* 1:Ready, 0:Timeout */
/* Timeout [ms] */

static int wait_ready (unsigned int timeout)
{
	uint8_t d;

	timeout /= 10;
	cli(); Timer2 = timeout; sei();
	do {
		d = SPI_Transmit(0xFF);
		cli(); timeout = Timer2; sei();
	} while (d != 0xFF && timeout);

	return (d == 0xFF) ? 1 : 0;
}


static void deselect (void)
{
	CS_HIGH();		/* Set CS# high */
	SPI_Transmit(0xFF);	/* Dummy clock (force DO hi-z for multiple slave SPI) */
}

static int select (void)	/* 1:Successful, 0:Timeout */
{
	CS_LOW();		/* Set CS# low */
	SPI_Transmit(0xFF);	/* Dummy clock (force DO enabled) */

	if (wait_ready(500)) return 1;	/* Leading busy check: Wait for card ready */

	deselect();		/* Timeout */
	return 0;
}

static uint8_t send_cmd (uint8_t cmd, uint32_t arg)
{
	uint8_t n, res;


	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card and wait for ready except to stop multiple block read */
	if (cmd != CMD12) {
		deselect();
		if (!select()) return 0xFF;
	}

	/* Send command packet */
	SPI_Transmit(0x40 | cmd);				/* Start + Command index */
	SPI_Transmit((uint8_t)(arg >> 24));		/* Argument[31..24] */
	SPI_Transmit((uint8_t)(arg >> 16));		/* Argument[23..16] */
	SPI_Transmit((uint8_t)(arg >> 8));			/* Argument[15..8] */
	SPI_Transmit((uint8_t)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) + Stop */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) Stop */
	SPI_Transmit(n);

	/* Receive command response */
	if (cmd == CMD12) SPI_Transmit(0xFF);		/* Skip a stuff byte when stop reading */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
	res = SPI_Transmit(0xFF);
	while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}

uint8_t mmc_disk_initialize()
{
	uint8_t n, cmd, ty, ocr[4];
	
	power_off();						/* Turn off the socket power to reset the card */
	_delay_ms(100);		/* Wait for 100ms */
	if (Stat & 0x02) return Stat;	/* No card in the socket? */

	power_on();							/* Turn on the socket power */
	FCLK_SLOW();
	for (n = 10; n; n--) SPI_Transmit(0xFF);	/* 80 dummy clocks */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* Put the card SPI mode */
		Timer1 = 100;						/* Initialization timeout of 1000 msec */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* Is the card SDv2? */
			for (n = 0; n < 4; n++) ocr[n] = SPI_Transmit(0xFF);	/* Get trailing return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* The card can work at vdd range of 2.7-3.6V */
				send_cmd(ACMD41, 1UL << 30);					/* Wait for leaving idle state (ACMD41 with HCS bit) */
				if (send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = SPI_Transmit(0xFF);
					ty = (ocr[0] & 0x40) ? 0x08 | 0x10 : 0x08;	/* Check if the card is SDv2 */
				}
			}
			} else {							/* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1) 	{
				ty = 0x04; cmd = ACMD41;	/* SDv1 */
				} else {
				ty = 0x01; cmd = CMD1;	/* MMCv3 */
			}
			while (Timer1 && send_cmd(cmd, 0));			/* Wait for leaving idle state */
			if (!Timer1 || send_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
			ty = 0;
		}
	}
	CardType = ty;
	deselect();

	if (ty) {			/* Initialization succeded */
		Stat &= ~0x01;		/* Clear STA_NOINIT */
		FCLK_FAST();
		} else {			/* Initialization failed */
		power_off();
	}

	return Stat;
}