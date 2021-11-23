#ifndef __IDE_H
#define	__IDE_H

#ifdef __cplusplus
extern "C"
{
#endif
	
	#include <hddio.h>
	#include <ctype.h>	
	#include <io.h>
	

	#define insl(port, buf, nr)  __asm__ ("cld;rep;insl	\n\t"::"d"(port), "D"(buf), "c"(nr))
	#define outsl(buf, nr, port) __asm__ ("cld;rep;outsl\n\t"::"d"(port), "S"(buf), "c"(nr))

	#define FST_FS	0x2e			/* normal partition */
	#define FST_SW	0x2f			/* swap partition */


	/* Registers */
	#define IDE_DATA_REGISTER				0x1f0
	#define IDE_ERROR_REGISTER				0x1f1
	#define IDE_FEATURE_REG					IDE_ERROR_REGISTER
	#define IDE_SECTOR_COUNT_REGISTER		0x1f2		//Number of sector which is read or write
	#define IDE_SECTOR_NUMBER_REGISTER		0x1f3
	#define IDE_CYLINDER_LOW_REGISTER		0x1f4
	#define IDE_CYLINDER_HIGH_REGISTER		0x1f5
	#define IDE_DRIVE_HEAD_REGISTER			0x1f6
	#define IDE_STATUS_REGISTER				0x1f7
	#define IDE_COMMAND_REGISTER			0x1f7
	#define IDE_DEVICE_CONTROL_REGISTER		0x3F6

	/* Drives */
	#define IDE_DRIVE_0						0xa0
	#define IDE_DRIVE_1						0xb0

	/* Commands */
	#define IDE_COMMAND_IDENTIFY_DRIVE		0xEC
	#define IDE_COMMAND_SEEK				0x70
	#define IDE_COMMAND_READ_SECTORS		0x21
	#define IDE_COMMAND_READ_BUFFER			0xE4
	#define IDE_COMMAND_WRITE_SECTORS		0x30
	#define IDE_COMMAND_WRITE_BUFFER		0xE8
	#define IDE_COMMAND_DIAGNOSTIC			0x90
	#define IDE_COMMAND_ATAPI_IDENT_DRIVE	0xA1

	/* Results words from Identify Drive Request */
	#define	IDE_INDENTIFY_NUM_CYLINDERS		0x01
	#define	IDE_INDENTIFY_NUM_HEADS			0x03
	#define	IDE_INDENTIFY_NUM_BYTES_TRACK	0x04
	#define	IDE_INDENTIFY_NUM_BYTES_SECTOR	0x05
	#define	IDE_INDENTIFY_NUM_SECTORS_TRACK	0x06

	/* bits of Status Register */
	#define IDE_STATUS_DRIVE_BUSY			0x80
	#define IDE_STATUS_DRIVE_READY			0x40
	#define IDE_STATUS_DRIVE_WRITE_FAULT	0x20
	#define IDE_STATUS_DRIVE_SEEK_COMPLETE	0x10
	#define IDE_STATUS_DRIVE_DATA_REQUEST	0x08
	#define IDE_STATUS_DRIVE_CORRECTED_DATA	0x04
	#define IDE_STATUS_DRIVE_INDEX			0x02
	#define IDE_STATUS_DRIVE_ERROR			0x01

	/* Bits of Device Control Register */
	#define IDE_DCR_NOINTERRUPT				0x02
	#define IDE_DCR_RESET					0x04
	
	/* Return codes from various IDE_* functions */
	#define	IDE_ERROR_NO_ERROR	0
	#define	IDE_ERROR_BAD_DRIVE	0
	#define	IDE_ERROR_INVALID_BLOCK	-2
	#define	IDE_ERROR_DRIVE_ERROR	-3
	
	/* Control register bits */
	#define IDE_CONTROL_REGISTER			0x3F6
	#define IDE_CONTROL_SOFTWARE_RESET		0x04
	#define IDE_CONTROL_INT_DISABLE			0x02	
	
	
	#define LOW_BYTE(x)		(x & 0xff)
	#define HIGH_BYTE(x)	((x >> 8) & 0xff)
	
	#define IDE_MAX_DRIVES					2

	enum IDECMDEnum{
		IDE_READ=0x20,
		IDE_WRITE=0x30,
		};
	
	typedef struct IDEDiskParametersTag{
	    short num_Cylinders;
    short num_Heads;
	    short num_SectorsPerTrack;
	    short num_BytesPerSector;
		short *Information;
	} IDEDisk;
	
	//Method
	short IDE_RW(unsigned int lba ,enum IDECMDEnum command,unsigned short NumSector ,void *buf );
	unsigned int IDE_getNumBlocks(int driveNum);
	short Init_IDE(void);

#ifdef __cplusplus
}
#endif

#endif
