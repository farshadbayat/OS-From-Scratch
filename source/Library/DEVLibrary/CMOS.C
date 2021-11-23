#include <cmos.h>
#include <io.h>

int cmos_read(byte address)
{
	while(cmos_busy());
	outportb(CMOS_OUT_PORT, 0x80|address);		// Use 0x80 coz we need to disable NMI
	return bcd2bin(inportb(CMOS_IN_PORT));
}

void cmos_write(byte address,word value)
{
	while(cmos_busy());
	outportb(CMOS_OUT_PORT,0x80|address); 
	outportb(CMOS_IN_PORT,value); 
}

bool cmos_busy()
{
	outportb(CMOS_OUT_PORT, 0x80|CMOS_STATUS_A);
	if(inportb(CMOS_IN_PORT) & 0x80)			// Bit 7 is set in Status Register A if Clock is busy
		return true;
	else
		return false;
}
//BCD to BIN Math
//Its easy to explain with an example
//Input : bin = 25 BCD
//		  bin = 0010 0101
//		  bin & 0xF = 0000 0101          -> (a)
//		  bin >> 4 = 0000 0010
//		  (bin >> 4) * 10 = 0001 0100    -> (b)
//		  (a) + (b) = 0001 1001 -> This is 25 in BINARY

