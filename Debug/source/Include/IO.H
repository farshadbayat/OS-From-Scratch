#ifndef __IO_H
#define	__IO_H

#ifdef __cplusplus
extern "C"
{
#endif

	#include <system.h>

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

#ifdef __cplusplus
}
#endif

#endif

