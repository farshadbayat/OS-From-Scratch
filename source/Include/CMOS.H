#ifndef __CMOS_H
#define	__CMOS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <ctype.h>

#define CMOS_OUT_PORT	0x70
#define CMOS_IN_PORT	0x71

#define CMOS_SECOND		0x00
#define CMOS_MINUTE		0x02
#define CMOS_HOUR		0x04
#define CMOS_DAY		0x06
#define CMOS_DATE		0x07
#define CMOS_MONTH		0x08
#define CMOS_YEAR		0x09
#define CMOS_STATUS_A	0x0A
#define CMOS_STATUS_B	0x0B
#define CMOS_STATUS_C	0x0C
#define CMOS_STATUS_D	0x0D

extern int  cmos_read(byte addr);
extern void cmos_write(byte addr,word val);
extern bool cmos_busy();

#ifdef __cplusplus
}
#endif

#endif

