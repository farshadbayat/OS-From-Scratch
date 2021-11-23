#ifndef __COMPONENT_H
#define	__COMPONENT_H

#ifdef __cplusplus
extern "C"
{
#endif

	#include <ctype.h>
	#include <color.h>	
	#include <ImageBmp.h>

	#define MAX_WINDOW	100
	#define MAX_FORM_NAME	40
	#define BORDER_LINE_SIZE	4 //Pixel
	#define BORDER_BAR_SIZE		22 //Pixel
	
	//typedef struct SizeTag{
	//	unsigned int Width;
	//	unsigned int Hight;
	//}Size;
	//typedef enum ControlTypeEnum ControlType;
	enum ControlType{
		ControlTypeForm,
		ControlTypeButton,
		ControlTypeLabel,
	};

	//typedef enum FormBorderStyleEnum FormBorderStyle;
	enum FormBorderStyle{
		FormBorderStyleNone,
		FormBorderStyleSizable,
		FormBorderStyleFixed3D,
	};

	typedef struct ControlFrameTag{//Baray Rasme Graphic Dar Abade Moshakhs
		 Point FPoint;
		 Size FSize;
	}ControlFrame;

	typedef struct ControlTag Control;
	 
	struct ControlTag{
		bool CUpdate;//For First Time Is False To Draw On Form		
		enum ControlType CType;
		Point CPoint;
		ControlFrame Frame;
		GraphicArea CGraphicArea;
	};

	typedef struct LabelControlTag LabelControl;
	struct LabelControlTag{
		Size CSize;		
		unsigned char *Text;
	};

	typedef struct FormControlTag FormControl;
	struct FormControlTag{
		Control *FControl;
		char *CName;				
		Point CPoint;
		Size CSize;
		ColorRGB BackColor;
		enum FormBorderStyle FormBorderStyle;
		bool TopMost;
		unsigned char *Text;
	};

	struct WINDLIST	{		
		unsigned int Handle;		
		unsigned long Flags;
		bool NeedsRepaint;
		FormControl *Form;

		struct WINDLIST *Prev, *Next;
		struct WINDLIST *First_Child, *Last_Child;
		struct WINDLIST *Parent;
	};
	
	
	extern struct WINDLIST **wm_handles;



	void GUIInit();
	void DumpWNDS();
	FormControl* InitForm(FormControl *NewForm ,Point CPoint ,Size CSize);
	void UpdateWNDS(struct WINDLIST *WND);
	bool CreatForm(FormControl *NewForm );

	

#ifdef __cplusplus
}
#endif


#endif
