#include <stdarg.h>

#ifndef __STDIO_H
#define	__STDIO_H

#ifdef __cplusplus
extern "C"
{
#endif

	//#include <keyboard.h>
	#include <math.h>
	#include <conio.h>
	#include <stdarg.h>
	#include <string.h>


	#define MAXLEN	200

	#define DEFULT_FLOATING_ZERO 6

	#define STDI_LA		0x1		//0000000000000001  Left Align
	#define STDI_SS		0x2		//0000000000000010  Sign Show
	#define STDI_ZP		0x4		//0000000000000100  Zero Pad
	#define STDI_BC		0x8		//0000000000001000  Blank Char
	#define STDI_NG		0x10	//0000000000100000  Negetive Number
	#define STDI_NP		0x20	//0000000001000000  Near Pointer(Defult is Far Pointer(~STDI_NP))

	#define STDI_Mh		0x30	//0000000010000000  short (16-bit) int(mode character=h)
	#define STDI_Ml		0x40	//0000000100000000  long (32-bit) int(mode character=l)
	#define STDI_ML		0x50	//0000001000000000  long long (64-bit) int(mode character=L)

	typedef int (*fnptr_t)(unsigned c, void **helper);

	//int printf(const char *fmt, ...);	
	//int vprintf_help(unsigned c, void **ptr);	
	//int vsprintf_help(unsigned c, void **ptr);
	//int vprintf(const char *fmt, va_list args);
	//int sprintf(char *buffer, const char *fmt, ...);	
	//int vsprintf(char *buffer, const char *fmt, va_list args);
	//int do_printf(const char *fmt, va_list args, fnptr_t fn, void *ptr);

	int printf(const char *fmt, ...);
	int cprintf(char Color,const char *frmt,...);
	int vsprintf(unsigned char *buffer,const char *frmt,va_list ap);
	int sprintf(unsigned char *buffer,const char *frmt,...);
	int vprintf(const char *fmt, va_list args);

	int GetIntegerNumber(unsigned short MaxLen,bool IsSplitterReturn);
	float GetFloatNumber(unsigned short MaxLen,bool IsSplitterReturn);
	char* GetString(char *Buffer ,unsigned short MaxLen,bool IsSplitterReturn);

#ifdef __cplusplus
}
#endif

#endif
