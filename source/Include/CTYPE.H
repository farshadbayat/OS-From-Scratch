#define _CTYPE_DISABLE_MACROS

#ifndef __CTYPE_H
#define __CTYPE_H

#ifdef __cplusplus
extern "C"
{
#endif
	
	typedef enum BooleanTag
	{
		FALSE =(char) 0,
		TRUE  =(char) 1	
	}boolean;

	#define false FALSE
	#define true TRUE
	#define bool boolean
	#define NOT(BooleanType)  BooleanType==false ? true : false

	#ifndef NULL
	//#define NULL ( (void*)0 )
	#define NULL ( 0 )
	#endif

	#ifndef ___SIZE_T
	typedef int size_t;
	#define	___SIZE_T
	#endif

	
	typedef unsigned long ulong;
	typedef unsigned int uint;
	typedef short int sint;
	typedef unsigned short int usint;
	typedef unsigned char byte;
	typedef unsigned short word;
	typedef unsigned int dword;
	//typedef unsigned short color;
	
//
// Character types
//

	#define _UPPER          0x1     // Upper case letter
	#define _LOWER          0x2     // Lower case letter
	#define _DIGIT          0x4     // Digit[0-9]
	#define _SPACE          0x8     // Tab, carriage return, newline, vertical tab or form feed
	#define _PUNCT          0x10    // Punctuation character
	#define _CONTROL        0x20    // Control character
	#define _BLANK          0x40    // Space char
	#define _HEX            0x80    // Hexadecimal digit
	
	#define _LEADBYTE       0x8000                      // Multibyte leadbyte
	#define _ALPHA          (0x0100 | _UPPER| _LOWER)   // Alphabetic character
	
	
	
	extern unsigned short *_pctype; // pointer to table for char's
	
	int _isctype(int c, int mask);
	
	int isspace(int c);
	int isupper(int c);
	int islower(int c);
	int isdigit(int c);
	int isxdigit(int c);
	int ispunct(int c);
	int isalpha(int c);
	int isalnum(int c);
	int isprint(int c);
	int isgraph(int c);
	int iscntrl(int c);
	int isleadbyte(int c);
	int toupper(int c);
	int tolower(int c);
	int  bcd2bin(byte b);
	byte bin2bcd(word b);
	
	

	#ifndef _CTYPE_DISABLE_MACROS 
		#define isalpha(c)     (_pctype[(c)] & (_UPPER | _LOWER))
		#define isupper(c)     (_pctype[(c)] & _UPPER)
		#define islower(c)     (_pctype[(c)] & _LOWER)
		#define isdigit(c)     (_pctype[(c)] & _DIGIT)
		#define isxdigit(c)    (_pctype[(c)] & _HEX)
		#define isspace(c)     (_pctype[(c)] & _SPACE)
		#define ispunct(c)     (_pctype[(c)] & _PUNCT)
		#define isalnum(c)     (_pctype[(c)] & (_UPPER | _LOWER | _DIGIT))
		#define isprint(c)     (_pctype[(c)] & (_BLANK | _PUNCT | _UPPER | _LOWER | _DIGIT))
		#define isgraph(c)     (_pctype[(c)] & (_PUNCT | _UPPER | _LOWER | _DIGIT))
		#define iscntrl(c)     (_pctype[(c)] & _CONTROL)
		#define isleadbyte(c)  (_pctype[(unsigned char)(c)] & _LEADBYTE)
		
		#define tolower(c)     ((c) - 'A' + 'a')
		#define toupper(c)     ((c) - 'a' + 'A')		
	#endif
/*		
 * These define types with unique names for each length. They make it easier to
 * port other environments.
 * 
 */

#ifdef __cplusplus
}
#endif

#endif
