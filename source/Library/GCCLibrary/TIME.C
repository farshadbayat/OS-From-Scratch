#include <time.h>
#include <cmos.h>
#include <stdio.h>
#include <conio.h>

void gettime(TIME *time)
{
	time -> second = cmos_read(CMOS_SECOND);
	time -> minute = cmos_read(CMOS_MINUTE);
	time -> hour   = cmos_read(CMOS_HOUR);
}

void settime(TIME *time)
{
	cmos_write(CMOS_SECOND,bin2bcd(time -> second));
 	cmos_write(CMOS_MINUTE,bin2bcd(time -> minute));
    cmos_write(CMOS_HOUR,bin2bcd(time -> hour));
}

void getdate(DATE *date)
{
	date -> year  = cmos_read(CMOS_YEAR);
	date -> month = cmos_read(CMOS_MONTH);
	date -> date  = cmos_read(CMOS_DATE);
	date -> day   = cmos_read(CMOS_DAY); 
}

void setdate(DATE *date)
{
	cmos_write(CMOS_YEAR,bin2bcd(date -> year));
	cmos_write(CMOS_MONTH,bin2bcd(date -> month));
	cmos_write(CMOS_DATE,bin2bcd(date -> date));
	cmos_write(CMOS_DAY,bin2bcd(date -> day));
}

void printtime()
{
	TIME time;
	DATE date;

	char days[7][5] = {"SUN "  , "MON ", "TUE ", "WED ", "THU ", "FRI ", "SAT "};
	char months[12][5] = {"JAN ", "FEB ", "MAR ", "APR ", "MAY ", "JUN ",
			      "JUL ", "AUG ", "SEP ", "OCT ", "NOV ", "DEC "};

	gettime(&time);
	getdate(&date);	
    printf("Local Time : %s  %s %d ", days[date.day], months[date.month - 1],date.date);

	if (time.hour % 12 == 0)
		printf("12");
	else
		printf(" %2d:%2d:%2d ",time.hour % 12,time.minute,time.second);
	if (time.hour >= 12)
		puts(" PM ");
	else
		puts(" AM ");
	printf(" %d ",date.year + 2000);
	puts("\t[ DONE ]\n");
}

