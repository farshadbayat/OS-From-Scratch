#ifndef __CONIO_H
#define	__CONIO_H

#ifdef __cplusplus
extern "C"
{
#endif

	#include <draw2d.h>
	#include <keyboard.h>
	#include <font.h>
	#include <graphics.h>
	/*#include <system.h>	
	#include <io.h>*/
	
	                        /* KEYBOARD SCANCODES */
	#define sEsc               1
	#define s1                 2
	#define s2                 3
	#define s3                 4
	#define s4                 5
	#define s5                 6
	#define s6                 7
	#define s7                 8
	#define s8                 9
	#define s9              0x0A
	#define s0              0x0B
	#define sMinus          0x0C            /* - _ */
	#define sEqual          0x0D            /* = + */
	#define sBackSpace      0x0E
	#define sTab            0x0F
	#define sQ              0x10
	#define sW              0x11
	#define sE              0x12
	#define sR              0x13
	#define sT              0x14
	#define sY              0x15
	#define sU              0x16
	#define sI              0x17
	#define sO              0x18
	#define sP              0x19
	#define sLBraket        0x1A          / * [ { */
	#define sRBraket        0x1B          / * ] } */
	#define sEnter          0x1C
	#define sCtrl           0x1D
	#define sA              0x1E
	#define sS              0x1F
	#define sD              0x20
	#define sF              0x21
	#define sG              0x22
	#define sH              0x23
	#define sJ              0x24
	#define sK              0x25
	#define sL              0x26
	#define sSemicolon      0x27            /* ; :  */
	#define sQuote          0x28            /* ' "  */
	#define sApostrophe     0x29            /* ` ~  */
	#define sLShift         0x2A
	#define sSlash          0x2B            /* \ |  */
	#define sZ              0x2C
	#define sX              0x2D
	#define sC              0x2E
	#define sV              0x2F
	#define sB              0x30
	#define sN              0x31
	#define sM              0x32
	#define sComma          0x33            /* , <  */
	#define sPoint          0x34            /* . >  */
	#define sBackSlash      0x35            /* / ?  */
	#define sRShift         0x36
	#define sAsteriks       0x37            /* *  on the keypad */
	#define sAlt            0x38
	#define sSpace          0x39
	#define sCapsLock       0x3A
	#define sF1             0x3B
	#define sF2             0x3C
	#define sF3             0x3D
	#define sF4             0x3E
	#define sF5             0x3F
	#define sF6             0x40
	#define sF7             0x41
	#define sF8             0x42
	#define sF9             0x43
	#define sF10            0x44
	#define sNumLock        0x45
	#define sScrollLock     0x46
	#define sHome           0x47
	#define sUp             0x48
	#define sPageUp         0x49
	#define sGrayMinus      0x4A            /* -  on the keypad */
	#define sLeft           0x4B
	#define sFive           0x4C            /* 5  on the keypad */
	#define sRight          0x4D
	#define sGrayPlus       0x4E            /* +  on the keypad */
	#define sEnd            0x4F
	#define sDown           0x50
	#define sPageDown       0x51
	#define sInsert         0x52
	#define sDelete         0x53
	#define sF11            0x57
	#define sF12            0x58
	
	
	#define _KEYBUF_SIZE    16
	
	typedef enum ColorTextModeTag{
			BLACK=0,
			BLUE=1,
			GREEN=2,
			CYAN=3,
			RED=4,
			MAGENTA=5,
			BROWN=6,
			LIGHTGRAY=7,
			DARKGRAY=8,
			LIGHTBLUE=9,
			LIGHTGREEN=10,
			LIGHTCYAN=11,
			LIGHTRED=12,
			LIGHTMAGENTA=13,
			YELLOW=14,
			WHITE=15
		}ColorTextMode;

	extern unsigned int X_Resolution,Y_Resolution;
	//extern unsigned short X_Resolution,Y_Resolution;
	extern short    _cursor_x, _cursor_y;
	extern char _keybuf[_KEYBUF_SIZE];
	extern int  _kbhead, _kbtail, _kbcnt;
	extern short _StartLine;
	extern ColorRGB GBackColor;
                        /* INNER DATA */
	/* CONSOLE.C */
	// Must be run first with a selector for B800h segment as a parameter
	void TextVideo_Install(void);
	// Updates location of a hardware text cursor
	void update_cursor(int row, int col);
	// Put string in console
	void puts(char *text);
	// Sets the forecolor and backcolor
	void settextcolor(unsigned char forecolor, unsigned char backcolor);
	void textcolor(char Color);
	char GetTextColor(void);
	void textbackground(char color);
	char GetBackColor (void);
	// Locates the cursor
	void gotoxy (short x, short y);
	// Reads cursor X
	short wherex();
	//short wherex_Grphic();
	// Reads cursor Y
	short wherey();
	//short wherey_Grphic();
	// Sets foreground color
	void textbackground (char color);
	// Sets foreground color
	void textcolor (char color);
	// Clears the screen with current color (_textattr)
	void clrscr();
	// Puts a character to the screen (adjusts cursor position)
	// Few control characters available:
	// \t, \n, \r, \b
	void putch (unsigned char c);
	// Returns non-zero result in case of pressed key
	int kbhit();
	// Returns scan code of the pressed key
	char getscan();
	// Returns ASCII code (in upper case) of the pressed key
	char getch();
	// Cleans the keyboard buffer
	void clear_keybuf();
	//update X & Y cursor
	void _update_cursor();
	char CharToUpper(char ch);
	char CharToLower(char ch);
	void StrToLower(char *Str,char *StrToLower);
	void StrToUpper(char *Str,char *StrToUpper);	
	
	
#ifdef __cplusplus
}
#endif
	
#endif

