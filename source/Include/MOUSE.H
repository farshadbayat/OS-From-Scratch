#ifndef __MOUSE_H
#define __MOUSE_H

#ifdef __cplusplus
extern "C"
{
#endif	
	#include <keyboard.h>
	#include <graphics.h>

	#define MOUSE_BUFFER_SIZE	10

	#define KEYCMD_SENDTO_MOUSE		0xd4
	#define MOUSECMD_ENABLE			0xf4

	//GraphicArea *MouseGraphicArea,*MouseOldGraphicArea;
	
	struct MouseDataRecive{
	   unsigned char Byte0;
	   unsigned char Byte1;
	   unsigned char Byte2;
	 };

	enum MouseBTNEnum{
		MouseBTNNone=0,
		MouseBTNLeft=0x1,
		MouseBTNRight=0x2,
		MouseBTNMiddle=0x4,
	};	

	struct MouseParameters{
		unsigned short X;
		unsigned short Y;
		signed char DX;
		signed char DY;
		bool OverFlowX;
		bool OverFlowY;
		enum MouseBTNEnum  Button;
		bool Hiden;
	};
	
	




	void MouseHandler(struct regs *r);
	void UpdateMouseBackGround(unsigned int X ,unsigned int Y);
	void HideMouse();
	void ShowMouse();
	bool IsMouseHiden();
	bool IsMouseShow();
	void Mouse_Install(unsigned short Width ,unsigned short Hight );
	unsigned char ReadMouse();
	void EnableMouse();
	void DisableMouse();
	void CheckPort();
	void DisableKeyboard();
	void EnableKeyboard();



#ifdef __cplusplus
}
#endif

#endif

