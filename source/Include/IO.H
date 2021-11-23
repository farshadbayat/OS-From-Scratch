#ifndef __IO_H
#define	__IO_H

#ifdef __cplusplus
extern "C"
{
#endif

	#include <ctype.h>

	#define sti enable
	#define cli disable 

	unsigned char inportb (unsigned short _port);
	unsigned int  inportw (unsigned short _port);
	void outportb (unsigned short _port, unsigned char _data);
	void outportw (unsigned short _port, unsigned int _data);
	void IO_Delay(void);
	bool IsInterruptsEnabled(void);
	unsigned disable(void);
	void enable(void);
	/*	My asm Declaration	*/
	 int inp8(int port);
	 int inp16(int port);
	 int inp32(int port);
	 void outp8(int port, int data);
	 void outp16(int port, int data);
	 void outp32(int port, int data);

#ifdef __cplusplus
}
#endif

#endif

