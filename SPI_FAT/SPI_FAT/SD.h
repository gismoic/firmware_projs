/*
 * SD.h
 *
 * Created: 7/13/2023 2:58:06 PM
 *  Author: osegh
 */ 


#ifndef SD_H_
#define SD_H_
#include <avr/io.h>
#include "SPI.h"
#include "MiSD.h"

typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR = 1,		/* 1: R/W Error */
	RES_WRPRT = 2,		/* 2: Write Protected */
	RES_NOTRDY = 3,		/* 3: Not Ready */
	RES_PARERR = 4		/* 4: Invalid Parameter */
} DRESULT;

typedef uint64_t QWORD;
typedef QWORD LBA_t;

uint8_t mmc_disk_initialize();
uint8_t mmc_disk_status (void);
DRESULT mmc_disk_read (uint8_t* buff, LBA_t sector, unsigned int count);
DRESULT mmc_disk_write (const uint8_t* buff, LBA_t sector, unsigned int count);
DRESULT mmc_disk_ioctl (uint8_t cmd, void* buff);
void mmc_disk_timerproc (void);



#endif /* SD_H_ */