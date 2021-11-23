//Notice: We can Find liner address with SVGA1->PhysBasePtr example 0xF0000F6B;

#ifndef __GRAPHICS_H
#define	__GRAPHICS_H

#ifdef __cplusplus
extern "C"
{
#endif

//#include <component.h>

#include <color.h>
#include <ctype.h>


#define M256	1
#define M16B	2
#define M24B	3
#define M32B	4
#define MS1K	11
#define MS2K	12
#define MS4K	14

	#define DESKTOP_BACKGROUND	(ColorRGB){152,78,0}//DEFULT DESKTOP BACKGROUND
	#define FORM_BACKGROUND		(ColorRGB){216,232,232}//DEFULT DESKTOP BACKGROUND


	

	typedef struct GraphicAreaTag{
		unsigned char  BitsPerPixel;
		unsigned char  BytePerPixel;
		unsigned int   width;
		unsigned int   hight;
		unsigned short *Buffer;
	}GraphicArea;
	
	extern GraphicArea MainGraphicArea;
	

	typedef enum TextModeTag{
		TEXT_80x25		=	0x003,
		TEXT_80x60		=	0x108,
		TEXT_80x50		=	0x1112,
		TEXT_132x25		=	0x109, 
		TEXT_132x43		=	0x10a, 
		TEXT_132x50		=	0x10b, 
		TEXT_132x60		=	0x10c, 
	}TextMods;
	typedef enum GraphicModeTag
	{     
		VGA_320x200_256		=	0x013, 
		SVGA_640x400_256 	=	0x100, 
		SVGA_640x480_256 	=	0x101, 
		SVGA_800x600_16 	=	0x102, 
		SVGA_800x600_256	=	0x103, 
		SVGA_1024x768_16 	=	0x104, 
		SVGA_1024x768_256 	=	0x105, 
		SVGA_1280x1024_16 	=	0x106, 
		SVGA_1280x1024_256	=	0x107,     
		SVGA_320x200_32k 	=	0x10d, 
		SVGA_320x200_64k	=	0x10e, 
		SVGA_320x200_16m	=	0x10f, 
		SVGA_640x480_32k	=	0x110, 
		SVGA_640x480_64k	=	0x111, 
		SVGA_640x480_16m	=	0x112, 
		SVGA_800x600_32k 	=	0x113, 
		SVGA_800x600_64k 	=	0x114, 
		SVGA_800x600_16m	=	0x115, 
		SVGA_1024x768_32k	=	0x116, 
		SVGA_1024x768_64k	=	0x117, 
		SVGA_1024x768_16m	=	0x118, 
		SVGA_1280x1024_32k	=	0x119, 
		SVGA_1280x1024_64k	=	0x11a, 
		SVGA_1280x1024_16m	=	0x11b,
		SVGA_1600x1200_256	=	0x11c,
		SVGA_1600x1200_32k	=	0x11d,
		SVGA_1600x1200_64k	=	0x11e, 
		SVGA_320x240_256	=	0x120,
		SVGA_320x240_64k	=	0x121,
		SVGA_400x300_256	=	0x122,
		SVGA_400x300_64k	=	0x123,  
	}GraphicMode;					 
		
	struct SVGA{
		unsigned short mode_attr;			/* Mode attributes                  */					
		unsigned char  winA_attr;			/* Window A attributes              */
		unsigned char  winB_attr;			/* Window B attributes              */
		unsigned short wingranularity;		/* Window granularity in k          */
		unsigned short winsize;				/* Window size in k                 */
		unsigned short winA_seg;			/* Window A segment                 */
		unsigned short winB_seg;			/* Window B segment                 */
				 void  (*func)();			/* Pointer to window function       */
		unsigned short bytesperscanline;	/* Bytes per scanline               */
		unsigned short Xresolution;			/* Horizontal resolution            */
		unsigned short Yresolution;			/* Vertical resolution              */
		unsigned char  Xcharsize;			/* Character cell width             */
		unsigned char  Ycharsize;			/* Character cell height            */
		unsigned char  numofplanes;			/* Number of memory planes          */
		unsigned char  bitsperpixel;		/* Bits per pixel                   */
		unsigned char  numberofbanks;		/* Number of CGA style banks        */
		unsigned char  memorymodel;			/* Memory model type                */
		unsigned char  banksize;			/* Size of CGA style banks          */
		unsigned char  numberofimagepages;	/* Number of images pages           */
		unsigned char  reserved1;			/* Reserved                         */
		unsigned char  redmasksize;			/* Size of direct color red mask    */
		unsigned char  redfieldposition;	/* Bit posn of lsb of red mask      */
		unsigned char  greenmasksize;		/* Size of direct color green mask  */
		unsigned char  greenfieldposition;	/* Bit posn of lsb of green mask    */
		unsigned char  bluemasksize;		/* Size of direct color blue mask   */
		unsigned char  bluefieldposition;	/* Bit posn of lsb of blue mask     */
		unsigned char  rsvdmasksize;		/* Size of direct color res mask    */
		unsigned char  RsvdFieldPosition;	/* Bit posn of lsb of res mask      */
		unsigned char  directcolormodeinfo;	/* Direct color mode attributes     */
		/* VESA 2.0 variables */
		unsigned long  PhysBasePtr;			/* physical address for flat frame buffer */
		unsigned long  OffScreenMemOffset;	/* pointer to start of off screen memory */
		unsigned short OffScreenMemSize;	/* amount of off screen memory in 1k units */
		unsigned char  res2[206]  ;			/* Pad to 256 byte block size       */
	}__attribute__((packed));

	struct SVGA_DETAIL{
		unsigned char  a[4];
		unsigned short ver;
		unsigned char  *inc;
		unsigned long  ability;
		unsigned short *table;
		unsigned short mem;
	}__attribute__((packed));

	typedef enum drawModeTag{
		Draw_Normal, 
		Draw_Reverse, 
		Draw_Or, 
		Draw_Xor, 
		Draw_Translucent
	} DrawMode;

	extern struct SVGA *SVGA1;
	extern struct SVGA_DETAIL *SVGADetail;
	//extern unsigned char g_RedMaskShift,g_GreenMaskShift,g_BlueMaskShift;

	/*Useful globals.*/
	extern unsigned short g_Xcenter;
	extern unsigned short g_Ycenter;
	extern unsigned short g_bytesperline;
	extern bool IsGraphic;
	extern unsigned char g_RedMaskShift,g_GreenMaskShift,g_BlueMaskShift;

	extern unsigned char  RGB_BLACK[3];
	extern unsigned char  RGB_WHITE[3];
	extern unsigned short page_num,_scr_width,no_of_pages,color_mode;
	extern char _signiture1[4];
	extern char _signiture2[4];

	//Function
	/* Initialize SVGA graphic system
	Return value:
	0: successful;
	1: video card does not support VESA SVGA graphic system;
	2: video card does not support the required graphic mode;
	3: unknown graphic mode,putpixel and getpixel functions are not enabled;
	4: not enough memory.*/
	//void (*_bios_setpage_func)();
	//extern void (*Mouse_Update_func)(unsigned int,unsigned int);
	extern void (*putpixel)(GraphicArea,unsigned int,unsigned int,ColorRGB ,DrawMode);
	extern int (*getpixel)(GraphicArea,unsigned int,unsigned int);
	bool GetSVGADetail(char *_info_buffer);
	int initgraph(GraphicMode _vmode);
	int RGBDrawPixel(unsigned short *VideoBuffer,ColorRGB *foreground,DrawMode mode, int xCoord, int yCoord);
	void cleardevice();
	void TextMode(TextMods TextMode);
	bool GraphicMoveClip(GraphicArea G_PTR,unsigned int DestX ,unsigned int DestY ,unsigned int SrcX ,unsigned int SrcY ,unsigned int Width ,unsigned int Hight);
	bool GraphicClearClip(GraphicArea G_PTR,unsigned int SrcX ,unsigned int SrcY ,unsigned int Width ,unsigned int Hight,ColorRGB Color);
	bool WindowGraphicMemoryCopy(GraphicArea MDest_PTR ,Point DestPoint ,GraphicArea MSrc_PTR ,Point SrcPoint ,Size WindowSize);
	Point POINT(unsigned int X,unsigned int Y);
	Size SIZE(unsigned int WIDTH,unsigned int HIGHT);
	bool WindowGraphicMemoryTestCopy(GraphicArea MDest_PTR ,Point DestPoint ,GraphicArea MSrc_PTR ,Point SrcPoint ,Size WindowSize);

#ifdef __cplusplus
}
#endif

#endif
