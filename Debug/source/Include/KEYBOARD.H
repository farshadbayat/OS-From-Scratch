#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#ifdef __cplusplus
extern "C"
{
#endif
	#include <system.h> 
	#include <conio.h>
	#include <io.h>
	#include <stdio.h>	

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////
	//		 KEYBOARD.C		 //
	///////////////////////////
/*
 * Flags
 */
	#define KEY_SPECIAL_FLAG 0x0100
	#define KEY_KEYPAD_FLAG  0x0200
	#define KEY_SHIFT_FLAG   0x1000
	#define KEY_ALT_FLAG     0x2000
	#define KEY_CTRL_FLAG    0x4000
	#define KEY_RELEASE_FLAG 0x8000

/*
 * Special key codes
 */
	#define _SPECIAL(num) (KEY_SPECIAL_FLAG | (num))
	#define KEY_UNKNOWN _SPECIAL(0)
	#define KEY_F1      _SPECIAL(1)
	#define KEY_F2      _SPECIAL(2)
	#define KEY_F3      _SPECIAL(3)
	#define KEY_F4      _SPECIAL(4)
	#define KEY_F5      _SPECIAL(5)
	#define KEY_F6      _SPECIAL(6)
	#define KEY_F7      _SPECIAL(7)
	#define KEY_F8      _SPECIAL(8)
	#define KEY_F9      _SPECIAL(9)
	#define KEY_F10     _SPECIAL(10)
	#define KEY_F11     _SPECIAL(11)
	#define KEY_F12     _SPECIAL(12)
	#define KEY_LCTRL   _SPECIAL(13)
	#define KEY_RCTRL   _SPECIAL(14)
	#define KEY_LSHIFT  _SPECIAL(15)
	#define KEY_RSHIFT  _SPECIAL(16)
	#define KEY_LALT    _SPECIAL(17)
	#define KEY_RALT    _SPECIAL(18)
	#define KEY_PRINTSCRN _SPECIAL(19)
	#define KEY_CAPSLOCK _SPECIAL(20)
	#define KEY_NUMLOCK _SPECIAL(21)
	#define KEY_SCRLOCK _SPECIAL(22)
	#define KEY_SYSREQ  _SPECIAL(23)

/*
 * Keypad keys
 */
	#define KEYPAD_START 128
	#define _KEYPAD(num) (KEY_KEYPAD_FLAG | KEY_SPECIAL_FLAG | (num+KEYPAD_START))
	#define KEY_KPHOME  _KEYPAD(0)
	#define KEY_KPUP    _KEYPAD(1)
	#define KEY_KPPGUP  _KEYPAD(2)
	#define KEY_KPMINUS _KEYPAD(3)
	#define KEY_KPLEFT  _KEYPAD(4)
	#define KEY_KPCENTER _KEYPAD(5)
	#define KEY_KPRIGHT _KEYPAD(6)
	#define KEY_KPPLUS  _KEYPAD(7)
	#define KEY_KPEND   _KEYPAD(8)
	#define KEY_KPDOWN  _KEYPAD(9)
	#define KEY_KPPGDN  _KEYPAD(10)
	#define KEY_KPINSERT _KEYPAD(11)
	#define KEY_KPDEL   _KEYPAD(12)

/*
 * ASCII codes for which there is no convenient C representation
 */
	#define ASCII_ESC 0x1B
	#define ASCII_BS  0x08
//Port Hardware	
	#define PORT_KEYDATA			0x0060
	#define PORT_KEYCMD				0x0064

	#define PORT_KEYSTA				0x0064
	#define KEYSTA_SEND_NOTREADY	0x02
	
    #define KEYCMD_ENABLE			0xAE
	#define KEYCMD_Disable			0xAD

	/*
	* Bits in status port
	*/
	#define KB_OUTPUT_FULL 0x01

	#define KB_KEY_RELEASE 0x80


	 extern unsigned short s_scanTableNoShift[128];
	 unsigned short* s_scanTable;


	 //void keyboard_handler(struct regs *r);
	 void WaitKeyBoardIsSendReady(void);
	 void KeyBoardLight(bool ScrollLock ,bool NumLock ,bool CapsLock );
	 void Keyboard_Install();
  

#ifdef __cplusplus
}
#endif

#endif

