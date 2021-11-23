#ifndef __TIME_H
#define	__TIME_H

#ifdef __cplusplus
extern "C"
{
#endif 
	
	#include <io.h>

	typedef struct time_str_tag
	{
		int hour;
		int minute;
		int second;
	}TIME;

	typedef struct date_str_tag
	{
		int year;
		int month;	
		int day;
		int date;
	}DATE;

	extern void gettime(TIME *t);
	extern void settime(TIME *t);
	extern void getdate(DATE *d);
	extern void setdate(DATE *d);
	extern void printtime();

#ifdef __cplusplus
}
#endif

#endif

