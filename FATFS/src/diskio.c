/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/


#include "diskio.h"		/* Declarations of disk functions */

/* Definitions of physical drive number for each drive */
//#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
//#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

#define DEV_SD		0    //0：最后作为盘符
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_SD :
//		result = RAM_disk_status();

		// translate the reslut code here
		return 0;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_SD :
		stat = SD_Initialize();
		return stat;
		// translate the reslut code here
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_SD:
		res = SD_ReadDisk(buff,sector,count);
		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_SD:
		res = SD_WriteDisk((u8*)buff,sector,count);
		while(res != 0)
		{
			res = SD_WriteDisk((u8*)buff,sector,count);
		}
		return res;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_SD:
		switch(cmd)
		{
			case CTRL_SYNC:
				res = RES_OK;
				break;
			case GET_SECTOR_SIZE:
				*(DWORD*)buff = 512;
				res = RES_OK;
				break;
			case GET_BLOCK_SIZE:
				*(WORD*)buff = 4096;
				res = RES_OK;
				break;
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = 1989632;
				res = RES_OK;
				break;
			default:
				res = RES_PARERR;
				break;
		}
		return res;
	}

	return RES_PARERR;
}



DWORD get_fattime (void)
{		
	DWORD date = 0;
	
	Rtc_GetValue();
	date =
	( (
	(RTC_DateStruct.RTC_Year+20) << 25) |
	(RTC_DateStruct.RTC_Month << 21 ) |
	(RTC_DateStruct.RTC_Date << 16 ) |
	(RTC_TimeStruct.RTC_Hours << 11 ) |
	(RTC_TimeStruct.RTC_Minutes << 5 ) |
	(RTC_TimeStruct.RTC_Seconds )
	);
	return date;
}




