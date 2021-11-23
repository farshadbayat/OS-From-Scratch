//Notice: We can Find liner address with SVGA1->PhysBasePtr example 0xF0000F6B;

#ifndef __DRAW3D_H
#define	__DRAW3D_H

#ifdef __cplusplus
extern "C"
{
#endif
	#include <graphics.h>
	#include <draw2d.h>

	void blend(GraphicArea G_PTR ,ColorRGB FirstColor,ColorRGB SecondColor ,int startX ,int startY ,int width ,int hight,bool IsVertical);


#ifdef __cplusplus
}
#endif

#endif
