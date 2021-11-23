//Notice: We can Find liner address with SVGA1->PhysBasePtr example 0xF0000F6B;

#ifndef __DRAW2D_H
#define	__DRAW2D_H

#ifdef __cplusplus
extern "C"
{
#endif
	
	#include <ImageBmp.h>
	#include <graphics.h>

	#define SPACE_BETWEN_CHAR	0//Pixel
	#define SPACE_BETWEN_LINE	0//Pixel

	typedef enum ImageModeTag{
		ImageMode_Normal,
		ImageMode_RepeatImage,
		ImageMode_StretchImage,
		ImageMode_AutoSize,
		ImageMode_CenterImage,
		ImageMode_Zoom,
	}ImageMode;

	int DrawLine(GraphicArea G_PTR ,ColorRGB *foreground,DrawMode mode, int startX, int startY,int endX, int endY);
	int DrawRect(GraphicArea G_PTR ,ColorRGB *foreground ,DrawMode mode ,int xCoord ,int yCoord ,int width ,int height ,int thickness );
	int DrawOval(GraphicArea G_PTR ,ColorRGB  *foreground, DrawMode mode, int centerX, int centerY, int width,int height, int thickness, int fill);
	void outtextxy(GraphicArea G_PTR ,dword x,dword y,char *title,unsigned int c);
	void drawChar(GraphicArea G_PTR ,dword x, dword y, dword index, ColorRGB Color,bool ClearLayout);
	void drawstring(GraphicArea G_PTR ,dword x,dword y,char *title,ColorRGB Color);
	int DrawImage(GraphicArea G_PTR ,image *drawImage ,DrawMode mode ,int xCoord ,int yCoord,int xOffset ,int yOffset ,int width ,int height,ImageMode ImageMode);
	void GetImage(GraphicArea G_PTR ,Point OffsetBuffer,image *drawImage ,DrawMode mode ,Point ImagePoint ,Size ImageSize);

	
#ifdef __cplusplus
}
#endif

#endif
